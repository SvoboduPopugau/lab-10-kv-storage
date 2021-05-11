// Copyright 2020 Your Name <your_email>

#ifndef STORAGE_STORAGEMANAGER_HPP
#define STORAGE_STORAGEMANAGER_HPP

#include <rocksdb/db.h>

#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <boost/log/trivial.hpp>
#include "MyQueue.hpp"
#include "CreateStorage.hpp"


struct Cell{
  size_t HandleNumber;
  std::string Key;
  std::string Value;
};



class StorageManager {
 public:
  StorageManager(std::string& input_filename, std::string& output_filename);
  ~StorageManager();
  void WriteValue(Cell&& KeyHash);
  void ParseInputDB();
  void Hash(Cell& cell);
  void WriteIntoOutputDB();


 private:
  bool ParseFlag_ = false;
  bool HashFlag_ = false;
  bool WriteFlag_ = false;
 private:
  std::mutex WriteMutex_;
  MyQueue<Cell> HashQueue_;
  MyQueue<Cell> WriteQueue_;
  std::string input_;
  std::string output_;
  std::vector<rocksdb::ColumnFamilyHandle*> fromHandles_;
  std::vector<rocksdb::ColumnFamilyHandle*> outHandles_;
  std::unique_ptr<rocksdb::DB> fromDb_ = nullptr;
  std::unique_ptr<rocksdb::DB> outputDb_ = nullptr;
  size_t numberOfThreads_ = std::thread::hardware_concurrency();
};

#endif  // STORAGEMANAGER_HPP
