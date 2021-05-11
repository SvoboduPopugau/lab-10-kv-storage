// Copyright 2020 Your Name <your_email>

#ifndef LOG_SETTINGS_HPP
#define LOG_SETTINGS_HPP

#include <string>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>


[[maybe_unused]]void Init_logger(const std::string& sev_level);

#endif  // LOG_SETTINGS_HPP
