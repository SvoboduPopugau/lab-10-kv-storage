#include <picosha2.h>
#include <rocksdb/db.h>

#include <iostream>

#include "CreateStorage.hpp"
#include "log_settings.hpp"
#include <rocksdb/db.h>

//int main() {
//  rocksdb::DB* db = nullptr;
//  rocksdb::Options options;
//
//  options.create_if_missing = true;
//
//  rocksdb::Status status = rocksdb::DB::Open(options, "TESTDB", &db);
//  assert(status.ok());
//
//  rocksdb::Status s = db->Put(rocksdb::WriteOptions(), "key1", "value1");
//  assert(s.ok());
//  s = db->Put(rocksdb::WriteOptions(), "key2", "value2");
//  assert(s.ok());
//
//  std::string val;
//  s = db->Get(rocksdb::ReadOptions(), "key1", &val);
//  assert(s.ok());
//
//  std::cout << val << std::endl;
//
//  status = db->Close();
//  delete db;
//}

int main(){

  std::string level = "trace";
  Init_logger(level);
  std::string directory = "TESTDB";
  CreateRandomDataBase(directory);
  TryOpen(directory);
}


