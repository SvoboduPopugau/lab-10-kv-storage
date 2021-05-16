// Copyright 2020 Your Name <your_email>

#ifndef STORAGE_STORAGEMANAGER_HPP_
#define STORAGE_STORAGEMANAGER_HPP_

#include <rocksdb/db.h>

#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <boost/log/trivial.hpp>
#include "MyQueue.hpp"
#include "CreateStorage.hpp"
#include <ThreadPool.h>
#include <vector>


struct Cell{
  size_t HandleNumber;
  std::string Key;
  std::string Value;
};



class StorageManager {
 public:
  StorageManager(std::string& input_filename, std::string& output_filename,
                 size_t number_of_threads);
  ~StorageManager();
  void WriteValue(Cell&& KeyHash);
  void ParseInputDB();
  void Hash(Cell& cell);
  void WriteIntoOutputDB();
  void MainWork();
  void HashParsed();

 private:
  bool ParseFlag_ = false;
  bool HashFlag_ = false;
  bool WriteFlag_ = false;

 private:
  MyQueue<Cell> HashQueue_;
  MyQueue<Cell> WriteQueue_;
  std::string input_;
  std::string output_;
  std::vector<rocksdb::ColumnFamilyHandle*> fromHandles_;
  std::vector<rocksdb::ColumnFamilyHandle*> outHandles_;
  rocksdb::DB* fromDb_ = nullptr;
  rocksdb::DB* outputDb_ = nullptr;
  size_t numberOfThreads_ = std::thread::hardware_concurrency();
  ThreadPool HashPool_;
};

#endif  // STORAGEMANAGER_HPP_
