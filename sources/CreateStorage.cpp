// Copyright 2020 Your Name <your_email>

#include <CreateStorage.hpp>
void CreateRandomDataBase(const std::string& directory) {
    const unsigned int NUMBER_OF_COLUMNS = 5;
    const unsigned int NUMBER_OF_VALUES = 50;

    try {
//    Create and Open DB
        rocksdb::Options options;
        options.create_if_missing = true;
        rocksdb::DB* db = nullptr;

        rocksdb::Status status = rocksdb::DB::Open(options, directory, &db);
        if (!status.ok())
            throw std::runtime_error{"DB::Open failed"};

//    Add ColumnFamilies in DB
        std::vector<std::string> names;
        names.reserve(NUMBER_OF_COLUMNS);
        for (unsigned i = 0; i < NUMBER_OF_COLUMNS; i++){
            names.emplace_back("ColumnFamily_" + std::to_string(i+1));
        }
        std::vector<rocksdb::ColumnFamilyHandle*> handles;
        status = db->CreateColumnFamilies(rocksdb::ColumnFamilyOptions(), names, &handles);
        if (!status.ok())
            throw std::runtime_error{"CreateColumnFamilies failed"};

//    Fill DB with random values
        std::string key{};
        std::string value{};
        for (unsigned c = 0; c < NUMBER_OF_COLUMNS; c++){
            for (unsigned int i = 0; i < NUMBER_OF_VALUES; i++){
                key = "key-" + std::to_string((c * NUMBER_OF_VALUES) + i);
                value = "value-" + std::to_string(rand()%100);
                status = db->Put(rocksdb::WriteOptions(), handles[c], rocksdb::Slice(key), rocksdb::Slice(value));
                if (!status.ok())
                    throw std::runtime_error{"Put [" + std::to_string(c) + "][" + std::to_string(i) + "] failed"};

                BOOST_LOG_TRIVIAL(trace) << "Added " << key << ": " << value << "to (" << c << ") column";
            }
            BOOST_LOG_TRIVIAL(trace) << "ColumnFamily [" << c << "] filled in";
        }

//        Destroy ColumnFamilies and close DB
        for (auto& x : handles){
            status = db->DestroyColumnFamilyHandle(x);
            if (!status.ok())
                throw std::runtime_error{"DestroyColumnFamily failed"};
        }

        delete db;
    } catch (std::exception& e) {
        BOOST_LOG_TRIVIAL(fatal) << e.what();
    }
    BOOST_LOG_TRIVIAL(trace) << "Random data base created";
}
