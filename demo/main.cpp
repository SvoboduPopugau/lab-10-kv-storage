#include <picosha2.h>
#include <rocksdb/db.h>

#include <iostream>

#include "CreateStorage.hpp"
#include "LogSettings.hpp"
#include "StorageManager.hpp"

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

//  std::string level = "trace";
//  Init_logger(level);
//  std::string directory = "TESTDB";
//  CreateRandomDataBase(directory);
//  TryOpen(directory);

  Cell* a = new Cell;
  a->HandleNumber = 2;
  a->Key = "key";
  a->Value = "val";

  std::unique_ptr<Cell> ptr = nullptr;

  ptr.reset(a);

  std::cout << ptr->Key << std::endl;
  std::cout << a->Value << std::endl;

  a = new Cell{9, "Hey", "Varvar"};

  std::cout << ptr->Value << std::endl;
  std::cout << a->Value << std::endl;
  delete a;

  std::vector<int> vec;
  vec.reserve(10);
  for(int i = 0; i < 10; i++){
    vec.push_back(i);
  }
  std::cout << vec.size();
}


