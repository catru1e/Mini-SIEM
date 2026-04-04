#pragma once

#include <vector>

#include "db_sqlite.hpp"
#include "json.hpp"

class DetectionEngine {
public:
  using json = nlohmann::json;

  explicit DetectionEngine(SqliteDb& db);

  std::vector<json> process_event(const json& event);

private:
  SqliteDb& db_;

  json detect_failed_login_by_ip(const json& event);
  json detect_failed_login_by_user(const json& event);

  json detect_invalid_user_by_ip(const json& event);
  json detect_suspicious_sudo_by_user(const json& event);
  json detect_too_frequent_logins_by_user(const json& event);
};
