#pragma once

#include "db_sqlite.hpp"
#include "json.hpp"

class CorrelationEngine {
public:
  explicit CorrelationEngine(SqliteDb& db);

  void process_event(const nlohmann::json& event);

private:
  SqliteDb& db_;

  void correlate_bruteforce_success(const nlohmann::json& event);
};
