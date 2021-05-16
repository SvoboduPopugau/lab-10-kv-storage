// Copyright 2020 Your Name <your_email>

#ifndef INCLUDE_CREATESTORAGE_HPP_
#define INCLUDE_CREATESTORAGE_HPP_

#include <string>
#include <rocksdb/db.h>
#include <boost/log/trivial.hpp>
#include <rocksdb/options.h>
#include <rocksdb/slice.h>
#include <rocksdb/advanced_options.h>
#include <picosha2.h>

void CreateRandomDataBase(const std::string& directory);

void TryOpen(const std::string& directory);

std::string sha256(const std::string& key, const std::string& value);


#endif // INCLUDE_CREATESTORAGE_HPP_
