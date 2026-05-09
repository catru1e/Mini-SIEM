#include "detect.hpp"

#include <iostream>
#include <chrono>
#include <ctime>
#include <fstream>
#include <sstream> //std::ostringstream
#include <vector>
#include <algorithm>
#include <utility>

using json  = nlohmann::json;

namespace {

std::string now_iso_utc() {
  using namespace std::chrono;
  auto now = system_clock::now();
  auto secs = time_point_cast<std::chrono::seconds>(now);
  auto ms = duration_cast<milliseconds>(now - secs).count();

  std::time_t t = system_clock::to_time_t(now);
  std::tm tm{};
  gmtime_r(&t, &tm);

  char buf[64];
  std::snprintf(buf, sizeof(buf),
	 	"%04d-%02d-%02dT%02d:%02d:%02d.%03lldZ",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                tm.tm_hour, tm.tm_min, tm.tm_sec,
                static_cast<long long>(ms));
  return std::string(buf);
}

std::string seconds_ago_iso_utc(int seconds) {
  using namespace std::chrono;
  auto now = system_clock::now() - std::chrono::seconds(seconds);
  auto secs = time_point_cast<std::chrono::seconds>(now);
  auto ms = duration_cast<milliseconds>(now - secs).count();
  std::time_t t = system_clock::to_time_t(now);
  std::tm tm{};
  gmtime_r(&t, &tm);

  char buf[64];
  std::snprintf(buf, sizeof(buf),
                "%04d-%02d-%02dT%02d:%02d:%02d.%03lldZ",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                tm.tm_hour, tm.tm_min, tm.tm_sec,
                static_cast<long long>(ms));
  return std::string(buf);
}

std::string safe_string(const json& j, const char* key, const std::string& def = "") {
  if (!j.contains(key) || j[key].is_null()) return def;
  if (j[key].is_string()) return j[key].get<std::string>();
  return j[key].dump();
}

std::string safe_field_string(const json& j, const std::string& key, const std::string& def = "") {
  if (key.empty()) return def;
  if (!j.contains(key) || j[key].is_null()) return def;
  if (j[key].is_string()) return j[key].get<std::string>();
  return j[key].dump();
}

std::string lower_copy(std::string s) {
  for (char& c : s) {
    if (c >= 'A' && c <= 'Z') {
      c = static_cast<char>(c - 'A' + 'a');
    }
  }
  return s;
}

bool contains_string(const std::vector<std::string>& values, const std::string& value) {
  return std::find(values.begin(), values.end(), value) != values.end();
}

std::vector<std::string> read_string_array(const json& j, const char* key) {
  std::vector<std::string> out;

  if (!j.contains(key)) return out;

  if (j[key].is_string()) {
    out.push_back(j[key].get<std::string>());
    return out;
  }

  if (!j[key].is_array()) return out;

  for (const auto& item : j[key]) {
    if (item.is_string()) {
      out.push_back(item.get<std::string>());
    }
  }

  return out;
}

std::string replace_all(std::string input, const std::string& from, const std::string& to) {
  if (from.empty()) return input;

  std::size_t pos = 0;
  while ((pos = input.find(from, pos)) != std::string::npos) {
    input.replace(pos, from.size(), to);
    pos += to.size();
  }

  return input;
}

std::string render_template(std::string text,
                            const json& event,
                            const std::string& group_key,
                            long long count,
                            int threshold,
                            int window_sec) {
  text = replace_all(text, "{{group_key}}", group_key);
  text = replace_all(text, "{{count}}", std::to_string(count));
  text = replace_all(text, "{{threshold}}", std::to_string(threshold));
  text = replace_all(text, "{{window_sec}}", std::to_string(window_sec));
  text = replace_all(text, "{{event_type}}", safe_string(event, "event_type", ""));
  text = replace_all(text, "{{source}}", safe_string(event, "source", ""));
  text = replace_all(text, "{{source_type}}", safe_string(event, "source_type", ""));
  text = replace_all(text, "{{host}}", safe_string(event, "host", ""));
  return text;
}

} //NAMESPACE END


DetectionEngine::DetectionEngine(SqliteDb& db, DetectionConfig config)
   : db_(db), config_(std::move(config)) {
  load_rules("config/rules.json");
}

void DetectionEngine::load_rules(const std::string& path) {
  rules_.clear();

  std::ifstream in(path, std::ios::binary);
  if (!in.is_open()) {
    std::cerr << "[detect] rules file not found: " << path << "\n";
    return;
  }

  try {
    json root;
    in >> root;

    if (!root.is_array()) {
      std::cerr << "[detect] rules file must contain JSON array: " << path << "\n";
      return;
    }

    for (const auto& item : root) {
      if (!item.is_object()) continue;

      DetectionRule rule = parse_rule(item);

      if (rule.id.empty()) {
        std::cerr << "[detect] skip rule without id\n";
        continue;
      }

      rules_.push_back(std::move(rule));
    }

    std::cerr << "[detect] loaded rules=" << rules_.size() << " from " << path << "\n";
  } catch (const std::exception& e) {
    std::cerr << "[detect] failed to load rules: " << e.what() << "\n";
  }
}

DetectionEngine::DetectionRule DetectionEngine::parse_rule(const json& item) const {
  DetectionRule rule;

  rule.id = safe_string(item, "id", "");
  rule.enabled = item.contains("enabled") && item["enabled"].is_boolean()
                 ? item["enabled"].get<bool>()
                 : true;

  rule.type = safe_string(item, "type", "threshold");

  rule.event_types = read_string_array(item, "event_types");
  rule.source_types = read_string_array(item, "source_types");

  if (rule.event_types.empty()) {
    const std::string one = safe_string(item, "event_type", "");
    if (!one.empty()) rule.event_types.push_back(one);
  }

  if (rule.source_types.empty()) {
    const std::string one = safe_string(item, "source_type", "");
    if (!one.empty()) rule.source_types.push_back(one);
  }

  rule.group_by = safe_string(item, "group_by", "");

  if (item.contains("threshold") && item["threshold"].is_number_integer()) {
    rule.threshold = item["threshold"].get<int>();
  }

  if (item.contains("window_sec") && item["window_sec"].is_number_integer()) {
    rule.window_sec = item["window_sec"].get<int>();
  }

  if (item.contains("suppress_sec") && item["suppress_sec"].is_number_integer()) {
    rule.suppress_sec = item["suppress_sec"].get<int>();
  }

  rule.severity = safe_string(item, "severity", "medium");
  rule.title = safe_string(item, "title", rule.id);
  rule.description = safe_string(item, "description", "");

  if (item.contains("conditions") && item["conditions"].is_array()) {
    for (const auto& cond : item["conditions"]) {
      if (cond.is_object()) {
        rule.conditions.push_back(parse_condition(cond));
      }
    }
  }

  if (rule.threshold <= 0) rule.threshold = 1;
  if (rule.window_sec <= 0) rule.window_sec = 60;
  if (rule.suppress_sec < 0) rule.suppress_sec = 0;

  return rule;
}

DetectionEngine::RuleCondition DetectionEngine::parse_condition(const json& item) const {
  RuleCondition condition;

  condition.field = safe_string(item, "field", "");
  condition.op = safe_string(item, "op", "equals");
  condition.value = safe_string(item, "value", "");
  condition.values = read_string_array(item, "values");

  return condition;
}

std::vector<json> DetectionEngine::process_event(const json& event) {
  std::vector<json> alerts;

  const std::string event_type = safe_string(event, "event_type", "");
  const std::string source_type = safe_string(event, "source_type", "");

  std::cerr << "[detect] process_event type=" << event_type
            << " source_type=" << source_type
            << " rules=" << rules_.size() << "\n";

  for (const auto& rule : rules_) {
    json alert = evaluate_rule(rule, event);
    if (!alert.is_null()) {
      alerts.push_back(std::move(alert));
    }
  }

  return alerts;
}

bool DetectionEngine::rule_matches_event(const DetectionRule& rule, const json& event) const {
  if (!rule.enabled) return false;

  const std::string event_type = safe_string(event, "event_type", "");
  const std::string source_type = safe_string(event, "source_type", "");

  if (!event_type_matches(rule, event_type)) return false;
  if (!source_type_matches(rule, source_type)) return false;
  if (!conditions_match(rule, event)) return false;

  return true;
}

bool DetectionEngine::event_type_matches(const DetectionRule& rule, const std::string& event_type) const {
  if (rule.event_types.empty()) return true;
  return contains_string(rule.event_types, event_type);
}

bool DetectionEngine::source_type_matches(const DetectionRule& rule, const std::string& source_type) const {
  if (rule.source_types.empty()) return true;
  return contains_string(rule.source_types, source_type);
}

bool DetectionEngine::conditions_match(const DetectionRule& rule, const json& event) const {
  for (const auto& condition : rule.conditions) {
    if (!condition_matches(condition, event)) {
      return false;
    }
  }

  return true;
}

bool DetectionEngine::condition_matches(const RuleCondition& condition, const json& event) const {
  if (condition.field.empty()) return false;

  const std::string field_value = safe_field_string(event, condition.field, "");
  const std::string op = lower_copy(condition.op);

  if (op == "exists") {
    return event.contains(condition.field) && !event[condition.field].is_null();
  }

  if (op == "equals") {
    return field_value == condition.value;
  }

  if (op == "not_equals") {
    return field_value != condition.value;
  }

  if (op == "contains") {
    return !condition.value.empty() &&
           field_value.find(condition.value) != std::string::npos;
  }

  if (op == "starts_with") {
    return !condition.value.empty() &&
           field_value.rfind(condition.value, 0) == 0;
  }

  if (op == "ends_with") {
    if (condition.value.empty()) return false;
    if (field_value.size() < condition.value.size()) return false;

    return field_value.compare(
      field_value.size() - condition.value.size(),
      condition.value.size(),
      condition.value
    ) == 0;
  }

  if (op == "in") {
    return contains_string(condition.values, field_value);
  }

  if (op == "contains_any") {
    for (const auto& value : condition.values) {
      if (!value.empty() && field_value.find(value) != std::string::npos) {
        return true;
      }
    }

    return false;
  }

  return false;
}

json DetectionEngine::evaluate_rule(const DetectionRule& rule, const json& event) {
  if (!rule_matches_event(rule, event)) {
    return nullptr;
  }

  std::string group_key;
  if (!rule.group_by.empty()) {
    group_key = safe_field_string(event, rule.group_by, "");
  }

  if (rule.type == "match") {
    if (group_key.empty()) {
      group_key = safe_string(event, "event_id", "");
    }

    if (rule.suppress_sec > 0 && !group_key.empty()) {
      const std::string suppression_since = seconds_ago_iso_utc(rule.suppress_sec);
      if (db_.has_recent_alert_for_rule_and_group_since(rule.id, group_key, suppression_since)) {
        std::cerr << "[detect] rule suppressed id=" << rule.id
                  << " group_key=" << group_key << "\n";
        return nullptr;
      }
    }

    return make_alert(rule, event, group_key, 1);
  }

  if (rule.type == "threshold") {
    if (rule.group_by.empty()) return nullptr;
    if (group_key.empty()) return nullptr;

    const std::string since_ts = seconds_ago_iso_utc(rule.window_sec);

    long long count = 0;
    if (rule.event_types.empty()) {
      const std::string current_type = safe_string(event, "event_type", "");
      count += db_.count_events_by_field_since(current_type, rule.group_by, group_key, since_ts);
    } else {
      for (const auto& t : rule.event_types) {
        count += db_.count_events_by_field_since(t, rule.group_by, group_key, since_ts);
      }
    }

    std::cerr << "[detect] rule id=" << rule.id
              << " group_by=" << rule.group_by
              << " group_key=" << group_key
              << " count=" << count
              << " threshold=" << rule.threshold
              << " since=" << since_ts << "\n";

    if (count < rule.threshold) {
      return nullptr;
    }

    if (rule.suppress_sec > 0) {
      const std::string suppression_since = seconds_ago_iso_utc(rule.suppress_sec);
      if (db_.has_recent_alert_for_rule_and_group_since(rule.id, group_key, suppression_since)) {
        std::cerr << "[detect] rule suppressed id=" << rule.id
                  << " group_key=" << group_key << "\n";
        return nullptr;
      }
    }

    return make_alert(rule, event, group_key, count);
  }

  return nullptr;
}

json DetectionEngine::make_alert(const DetectionRule& rule,
                                 const json& event,
                                 const std::string& group_key,
                                 long long count) const {
  const std::string now = now_iso_utc();

  std::string description = rule.description;
  if (description.empty()) {
    description = "Rule {{rule_id}} matched for {{group_key}}.";
  }

  description = render_template(
    description,
    event,
    group_key,
    count,
    rule.threshold,
    rule.window_sec
  );

  description = replace_all(description, "{{rule_id}}", rule.id);

  json alert = {
    {"ts", now},
    {"rule_id", rule.id},
    {"rule_name", rule.id},
    {"severity", rule.severity},
    {"title", rule.title},
    {"description", description},
    {"source_event_id", safe_string(event, "event_id", "")},
    {"source_type", safe_string(event, "source_type", "")},
    {"source", safe_string(event, "source", "")},
    {"event_type", safe_string(event, "event_type", "")},
    {"group_by", rule.group_by},
    {"group_key", group_key},
    {"count", count},
    {"threshold", rule.threshold},
    {"window_seconds", rule.window_sec},
    {"suppress_seconds", rule.suppress_sec},
    {"host", safe_string(event, "host", "")}
  };

  if (event.contains("src_ip")) alert["src_ip"] = event["src_ip"];
  if (event.contains("user")) alert["user"] = event["user"];
  if (event.contains("process_name")) alert["process_name"] = event["process_name"];
  if (event.contains("cmdline")) alert["cmdline"] = event["cmdline"];
  if (event.contains("path")) alert["path"] = event["path"];
  if (event.contains("program")) alert["program"] = event["program"];
  if (event.contains("raw")) alert["raw"] = event["raw"];

  return alert;
}
