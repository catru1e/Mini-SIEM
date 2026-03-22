#pragma once

#include "db_sqlite.hpp"
#include "json.hpp"

class DetectionEngine {
public:
  explicit DetectionEngine(SqliteDb& db);

  void process_event(const nlohmann::json& event);

private:
  SqliteDb& db_;

  void detect_failed_login_by_ip(const nlohmann::json& event);
  void detect_failed_login_by_user(const nlohmann::json& event);

  void detect_invalid_user_by_ip(const nlohmann::json& event);
  void detect_suspicious_sudo_by_user(const nlohmann::json& event);
  void detect_too_frequent_logins_by_user(const nlohmann::json& event);
};
