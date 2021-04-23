// Copyright 2020 Your Name <your_email>

#include <CreateStorage.hpp>
#include <iostream>
void CreateRandomDataBase(const std::string& directory) {
  const unsigned int NUMBER_OF_COLUMNS = 5;
  const unsigned int NUMBER_OF_VALUES = 50;

  try {
    //    Create and Open DB
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::DB* db = nullptr;

    rocksdb::Status status = rocksdb::DB::Open(options, directory, &db);
    if (!status.ok()) throw std::runtime_error{"DB::Open failed"};
    BOOST_LOG_TRIVIAL(debug) << "Random database opened";

    //    Add ColumnFamilies in DB
    std::vector<std::string> names;
    names.reserve(NUMBER_OF_COLUMNS);
    for (unsigned i = 0; i < NUMBER_OF_COLUMNS; i++) {
      names.emplace_back("ColumnFamily_" + std::to_string(i + 1));
    }
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    status = db->CreateColumnFamilies(rocksdb::ColumnFamilyOptions(), names,
                                      &handles);
    if (!status.ok()) throw std::runtime_error{"CreateColumnFamilies failed"};

    //    Fill DB with random values
    std::string key{};
    std::string value{};
    for (unsigned c = 0; c < NUMBER_OF_COLUMNS; c++) {
      for (unsigned int i = 0; i < NUMBER_OF_VALUES; i++) {
        key = "key-" + std::to_string((c * NUMBER_OF_VALUES) + i);
        value = "value-" + std::to_string(rand() % 100);
        status = db->Put(rocksdb::WriteOptions(), handles[c],
                         rocksdb::Slice(key), rocksdb::Slice(value));
        if (!status.ok())
          throw std::runtime_error{"Put [" + std::to_string(c + 1) + "][" +
                                   std::to_string(i) + "] failed"};

        BOOST_LOG_TRIVIAL(trace) << "Added [" << key << "]:[" << value
                                 << "] to (" << c + 1 << ") column";
      }
      BOOST_LOG_TRIVIAL(debug)
          << "ColumnFamily \"" << handles[c]->GetName() << "\" filled in";
    }

    //        Destroy ColumnFamilies and close DB
    for (auto& x : handles) {
      status = db->DestroyColumnFamilyHandle(x);
      if (!status.ok()) throw std::runtime_error{"DestroyColumnFamily failed"};
    }

    //    names.clear();
    //    rocksdb::DB::ListColumnFamilies(rocksdb::DBOptions(), directory,
    //    &names); for(size_t i = 0; i < names.size(); i++){
    //      if (names[i] == rocksdb::kDefaultColumnFamilyName)
    //        names.erase(names.begin() + i);
    //      std::cout << names[i] << "  ";
    //    }

    delete db;
    BOOST_LOG_TRIVIAL(debug) << "Random data base created";

  } catch (std::exception& e) {
    BOOST_LOG_TRIVIAL(fatal) << e.what();
  }
}

void TryOpen(const std::string& directory) {
  try {
    rocksdb::Options options;
    rocksdb::DB* db;
    std::vector<std::string> names;
    std::vector<rocksdb::ColumnFamilyDescriptor> descriptors;
    rocksdb::Status s = rocksdb::DB::ListColumnFamilies(rocksdb::DBOptions(), directory, &names);
    if (!s.ok())
      throw std::runtime_error{"ListColumn failed"};

    descriptors.reserve(names.size());
    for (auto& x : names) {
      descriptors.emplace_back(x, rocksdb::ColumnFamilyOptions());
    }

    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    s = rocksdb::DB::OpenForReadOnly(
        rocksdb::DBOptions(), directory, descriptors, &handles, &db);
    if (!s.ok()) throw std::runtime_error{"DB::OpenForReadOnly failed"};

    rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions(), handles[1]);
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
      BOOST_LOG_TRIVIAL(trace)
          << it->key().ToString() << ":" << it->value().ToString() << " can be read";
    }
    if (!it->status().ok()) throw std::runtime_error("Iterator failed");
    delete it;

    for (auto& x : handles) {
      s = db->DestroyColumnFamilyHandle(x);
      if (!s.ok()) throw std::runtime_error{"DestroyColumnFamily failed"};
    }
    delete db;

  } catch (std::exception& e) {
    BOOST_LOG_TRIVIAL(fatal) << e.what();
  }
}
