#include "log_settings.hpp"

boost::log::trivial::severity_level choose_sev_level(
    const std::string& sev_level) {
  if (sev_level == "trace")
    return boost::log::trivial::severity_level::trace;
  else if (sev_level == "debug")
    return boost::log::trivial::severity_level::debug;
  else if (sev_level == "info")
    return boost::log::trivial::severity_level::info;
  else if (sev_level == "warning")
    return boost::log::trivial::severity_level::warning;
  else
    return boost::log::trivial::severity_level::error;
}

void Init_logger(const std::string& sev_level) {
  boost::log::add_common_attributes();

  boost::log::core::get()->set_filter(boost::log::trivial::severity >=
                                      choose_sev_level(sev_level));

  boost::log::add_console_log(
      std::clog, boost::log::keywords::format =
                     "[%TimeStamp%][%ThreadID%][%Severity%]: %Message%");

  boost::log::add_file_log(
      boost::log::keywords::file_name = "Sample_%N.log",
      boost::log::keywords::rotation_size = 20 * 1024 * 1024,
      boost::log::keywords::time_based_rotation =
          boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
      boost::log::keywords::format = "[%TimeStamp%][%Severity%]: %Message%");
}
