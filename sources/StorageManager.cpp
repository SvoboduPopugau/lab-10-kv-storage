#include "StorageManager.hpp"

StorageManager::StorageManager(std::string& input_filename,
                               std::string& output_filename)
    : WriteMutex_(),
      HashQueue_(),
      WriteQueue_(),
      input_(input_filename),
      output_(output_filename) {
  rocksdb::DB* db = nullptr;
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
                                     &fromHandles_, &db);
    if (!s.ok())
      throw std::runtime_error("OpenForReadOnly of input DB is failed");
    else {
      fromDb_.reset(db);
      db = nullptr;
    }

    names.erase(names.begin());
    //    desc.clear();

    rocksdb::Options options;
    options.create_if_missing = true;

    s = rocksdb::DB::Open(options, output_, &db);
    if (!s.ok())
      throw std::runtime_error("Open of output DB is failed");
    else {
      outputDb_.reset(db);
      db = nullptr;
    }

    outputDb_->CreateColumnFamilies(rocksdb::ColumnFamilyOptions(), names,
                                    &outHandles_);

    outHandles_.insert(outHandles_.begin(), outputDb_->DefaultColumnFamily());

  } catch (std::exception& e) {
    BOOST_LOG_TRIVIAL(fatal) << e.what();
  }
}
StorageManager::~StorageManager() {
  try{
    rocksdb::Status s;
    if (!fromHandles_.empty() && fromDb_ != nullptr){
      for(auto& x : fromHandles_){
        s = fromDb_->DestroyColumnFamilyHandle(x);
        if(!s.ok()) {
          throw std::runtime_error("Destroy From Handle failed in destructor");
        }
      }
//      fromDb_.get_deleter();
    }

    if (!outHandles_.empty() && outputDb_ != nullptr){
      for (auto& x: outHandles_){
        s = outputDb_->DestroyColumnFamilyHandle(x);
        if (!s.ok()){
          throw std::runtime_error("Destroy Output Handle failed in destructor");
        }
      }
//      outputDb_.get_deleter();
    }

  } catch (std::exception& e) {
    BOOST_LOG_TRIVIAL(fatal) << e.what();
  }
}
void StorageManager::WriteValue(Cell&& KeyHash) {
  try {
    rocksdb::Status s = outputDb_->Put(rocksdb::WriteOptions(),
                                       outHandles_[KeyHash.HandleNumber],
                                       KeyHash.Key, KeyHash.Value);
    if (!s.ok()) {
      throw std::runtime_error("Writing in output DB is failed");
    }
  } catch (std::exception& e) {
    BOOST_LOG_TRIVIAL(fatal) << e.what();
  }
}
void StorageManager::ParseInputDB() {
  rocksdb::Iterator* it;
  for (size_t i = 0; i < fromHandles_.size(); ++i){
    it = fromDb_->NewIterator(rocksdb::ReadOptions(), fromHandles_[i]);
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
      HashQueue_.Push({i, it->key().ToString(), it->value().ToString()});
      BOOST_LOG_TRIVIAL(trace)
          << it->key().ToString() << ":" << it->value().ToString()
          << " parsed from " << input_;
    }
  }
  delete it;
  ParseFlag_ = true;
}
void StorageManager::Hash(Cell& cell) {
  WriteQueue_.Push({cell.HandleNumber, cell.Key, sha256(cell.Key, cell.Value)});
}
void StorageManager::WriteIntoOutputDB() {
  Cell item;
  while(!WriteQueue_.Empty() || !HashFlag_){
    if(WriteQueue_.Pop(item)){
      WriteValue(std::move(item));
    }
  }
  WriteFlag_ = true;
}

