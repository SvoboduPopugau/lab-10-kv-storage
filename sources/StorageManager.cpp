// Copyright 2020 Your Name <your_email>
#include "StorageManager.hpp"

#include <iostream>

StorageManager::StorageManager(std::string& input_filename,
                               std::string& output_filename,
                               size_t number_of_threads)
    : HashQueue_(),
      WriteQueue_(),
      input_(input_filename),
      output_(output_filename),
      numberOfThreads_(number_of_threads),
      HashPool_(number_of_threads) {
  rocksdb::Status s{};
  std::vector<std::string> names;
  std::vector<rocksdb::ColumnFamilyDescriptor> desc;
  try {
    s = rocksdb::DB::ListColumnFamilies(rocksdb::DBOptions(), input_, &names);
    if (!s.ok()) throw std::runtime_error("ListColumnFamilies is failed");

    desc.reserve(names.size());
    for (auto& x : names) {
      desc.emplace_back(x, rocksdb::ColumnFamilyOptions());
    }

    s = rocksdb::DB::OpenForReadOnly(rocksdb::DBOptions(), input_, desc,
                                     &fromHandles_, &fromDb_);
    if (!s.ok())
      throw std::runtime_error("OpenForReadOnly of input DB is failed");

    names.erase(names.begin());
    //    desc.clear();

    rocksdb::Options options;
    options.create_if_missing = true;

    s = rocksdb::DB::Open(options, output_, &outputDb_);
    if (!s.ok()) throw std::runtime_error("Open of output DB is failed");

    outputDb_->CreateColumnFamilies(rocksdb::ColumnFamilyOptions(), names,
                                    &outHandles_);

    outHandles_.insert(outHandles_.begin(), outputDb_->DefaultColumnFamily());

    std::cout << "Created output" << std::endl;
  } catch (std::exception& e) {
    BOOST_LOG_TRIVIAL(fatal) << e.what();
  }
}
StorageManager::~StorageManager() {
  try {
    rocksdb::Status s;
    if (!fromHandles_.empty() && fromDb_ != nullptr) {
      for (auto& x : fromHandles_) {
        s = fromDb_->DestroyColumnFamilyHandle(x);
        if (!s.ok()) {
          throw std::runtime_error("Destroy From Handle failed in destructor");
        }
      }
      fromHandles_.clear();
      s = fromDb_->Close();
      if (!s.ok()) {
        throw std::runtime_error("Closing of fromDB in destructor");
      }
      delete fromDb_;
    }

    if (!outHandles_.empty() && outputDb_ != nullptr) {
      for (auto& x : outHandles_) {
        s = outputDb_->DestroyColumnFamilyHandle(x);
        if (!s.ok()) {
          throw std::runtime_error(
              "Destroy Output Handle failed in destructor");
        }
      }
      outHandles_.clear();
      //      s = outputDb_->Close();
      //      if(!s.ok())
      //        throw std::runtime_error("Closing of outputDB in destructor");
      //      delete outputDb_;
    }
    BOOST_LOG_TRIVIAL(debug) << "Destruction of StorageManager";
  } catch (std::exception& e) {
    BOOST_LOG_TRIVIAL(fatal) << e.what();
  }
}
void StorageManager::WriteValue(Cell&& KeyHash) {
  try {
    rocksdb::Status s = outputDb_->Put(rocksdb::WriteOptions(),
                                       outHandles_[KeyHash.HandleNumber],
                                       KeyHash.Key, KeyHash.Value);
    BOOST_LOG_TRIVIAL(trace)
        << KeyHash.Key << ":" << KeyHash.Value << " Writen to output DB";
    if (!s.ok()) {
      throw std::runtime_error("Writing in output DB is failed");
    }
  } catch (std::exception& e) {
    BOOST_LOG_TRIVIAL(fatal) << e.what();
  }
}
void StorageManager::ParseInputDB() {
  std::vector<rocksdb::Iterator*> iterators;
  rocksdb::Iterator* it;

  for (size_t i = 0; i < fromHandles_.size(); ++i) {
    it = fromDb_->NewIterator(rocksdb::ReadOptions(), fromHandles_[i]);
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
      HashQueue_.Push({i, it->key().ToString(), it->value().ToString()});
      BOOST_LOG_TRIVIAL(trace)
          << it->key().ToString() << ":" << it->value().ToString()
          << " parsed from input DB";
    }
    iterators.emplace_back(it);
    it = nullptr;
  }

  for (auto& x : iterators) {
    delete x;
  }

  ParseFlag_ = true;
  BOOST_LOG_TRIVIAL(debug) << "Parsing of input DB finished";
}
void StorageManager::Hash(Cell& cell) {
  WriteQueue_.Push({cell.HandleNumber, cell.Key, sha256(cell.Key, cell.Value)});
}
void StorageManager::WriteIntoOutputDB() {
  Cell item;
  while (!WriteQueue_.Empty() || !HashFlag_) {
    if (WriteQueue_.Pop(item)) {
      WriteValue(std::move(item));
    }
  }
  WriteFlag_ = true;
  BOOST_LOG_TRIVIAL(debug) << "Writing into output DB finished";
}
void StorageManager::MainWork() {
  std::thread parseThread([this]() { ParseInputDB(); });

  std::thread writeThread([this]() { WriteIntoOutputDB(); });

  parseThread.join();
  HashParsed();
  writeThread.join();

  while (!HashFlag_ || !ParseFlag_ || !WriteFlag_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  BOOST_LOG_TRIVIAL(debug) << "Main Work completed ";
}
void StorageManager::HashParsed() {
  Cell item;
  while (!ParseFlag_ || !HashQueue_.Empty()) {
    if (HashQueue_.Pop(item)) {
      HashPool_.enqueue([this](Cell x) { Hash(x); }, item);
    }
  }
  HashFlag_ = true;
  BOOST_LOG_TRIVIAL(debug) << "Hashing is finished ";
}
