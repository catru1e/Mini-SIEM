#pragma once

#include <string>
#include "json.hpp"

class SyslogParser {
public:
  nlohmann::json parse(const std::string& line,
  const std::string& source_name) const;

private:
  static std::string detect_program(const std::string& line);
  static std::string detect_message(const std::string& line);
  static std::string detect_severity(const std::string& line);
};
