#pragma once

#include <vector>

#include "db_sqlite.hpp"
#include "json.hpp"

class CorrelationEngine {
public:
  using json = nlohmann::json;

  explicit CorrelationEngine(SqliteDb& db);

  std::vector<json> process_event(const json& event);

private:
  SqliteDb& db_;

  json correlate_bruteforce_success(const json& event);
};
