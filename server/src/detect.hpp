#pragma once

#include <string>
#include <vector>

#include "db_sqlite.hpp"
#include "json.hpp"
#include "config.hpp"

class DetectionEngine {
public:
  using json = nlohmann::json;

  explicit DetectionEngine(SqliteDb& db, DetectionConfig config);

  std::vector<json> process_event(const json& event);

private:
  struct RuleCondition {
    std::string field;
    std::string op;
    std::string value;
    std::vector<std::string> values;
  };

  struct DetectionRule {
    std::string id;
    bool enabled = true;
    std::string type = "threshold";

    std::vector<std::string> event_types;
    std::vector<std::string> source_types;
    std::vector<RuleCondition> conditions;

    std::string group_by;
    int threshold = 1;
    int window_sec = 60;
    int suppress_sec = 0;

    std::string severity = "medium";
    std::string title = "Detection rule matched";
    std::string description;
  };

  SqliteDb& db_;
  DetectionConfig config_;
  std::vector<DetectionRule> rules_;

  void load_rules(const std::string& path);
  DetectionRule parse_rule(const json& item) const;
  RuleCondition parse_condition(const json& item) const;

  bool rule_matches_event(const DetectionRule& rule, const json& event) const;
  bool event_type_matches(const DetectionRule& rule, const std::string& event_type) const;
  bool source_type_matches(const DetectionRule& rule, const std::string& source_type) const;
  bool conditions_match(const DetectionRule& rule, const json& event) const;
  bool condition_matches(const RuleCondition& condition, const json& event) const;

  json evaluate_rule(const DetectionRule& rule, const json& event);
  json make_alert(const DetectionRule& rule,
                  const json& event,
                  const std::string& group_key,
                  long long count) const;
};
