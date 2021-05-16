#include <picosha2.h>
#include <rocksdb/db.h>

#include <iostream>

#include "CreateStorage.hpp"
#include "LogSettings.hpp"
#include "StorageManager.hpp"

int main(){

  std::string level = "trace";
  Init_logger(level);
  std::string directory = "TESTDB";
  CreateRandomDataBase(directory);
//  std::string output = "OUTPUT";
//
//  StorageManager Man(directory, output, 4);
//  Man.MainWork();
}


