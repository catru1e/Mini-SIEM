#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include <deque>
#include <memory>
#include <random>
#include <cstdlib>
#include <regex>

#include "httplib.h"
#include "json.hpp"
#include "db_sqlite.hpp"
#include "detect.hpp"
#include "correlate.hpp"
#include "config.hpp"
#include "auth_db.hpp"
#include "session_store.hpp"

using json = nlohmann::json;

static std::string now_iso_utc() {
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

static std::string safe_string(const json& j, const char* key, const std::string& def = "") {
  if (!j.contains(key) || j[key].is_null()) return def;
  if (j[key].is_string()) return j[key].get<std::string>();
  // если не строка — сериализуем в строку
  return j[key].dump();
}

static std::string lower_copy(std::string s) {
  for (char& c : s) {
    if (c >= 'A' && c <= 'Z') {
      c = static_cast<char>(c - 'A' + 'a');
    }
  }
  return s;
}

static std::string generate_event_id() {
  static const char* hex = "0123456789abcdef";

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(0, 15);
  std::uniform_int_distribution<int> variant_dist(8, 11);

  std::string id = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";

  for (char& c : id) {
    if (c == 'x') {
      c = hex[dist(gen)];
    } else if (c == 'y') {
      c = hex[variant_dist(gen)];
    }
  }

  return id;
}

static bool is_auth_event_type(const std::string& event_type) {
  const std::string t = lower_copy(event_type);

  return t == "failed_login" ||
         t == "accepted_login" ||
         t == "invalid_user" ||
         t == "privilege_escalation" ||
         t == "session_open" ||
         t == "session_close" ||
         t == "auth_failed" ||
         t == "auth_success" ||
         t == "auth_invalid_user";
}

static std::string detect_source_type(const json& event) {
  const std::string source = lower_copy(safe_string(event, "source", ""));
  const std::string event_type = lower_copy(safe_string(event, "event_type", ""));

  if (source == "proc" || event_type == "process_start") {
    return "process";
  }

  if (source.rfind("inotify", 0) == 0 ||
      event_type.rfind("file_", 0) == 0 ||
      event.contains("watched_path")) {
    return "file";
  }

  if (source.find("auth") != std::string::npos ||
      source.find("demo_auth") != std::string::npos ||
      is_auth_event_type(event_type)) {
    return "auth";
  }

  if (source.find("syslog") != std::string::npos ||
      source.find("kern") != std::string::npos ||
      event_type == "system_event") {
    return "syslog";
  }

  return "syslog";
}

static bool read_file(const std::string& path, std::string& out) {
  std::ifstream file(path, std::ios::binary);
  if (!file) return false;

  std::ostringstream ss;
  ss << file.rdbuf();
  out = ss.str();
  return true;
}

static bool read_json_file(const std::string& path, json& out, std::string& err) {
  try {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
      err = "failed to open file: " + path;
      return false;
    }

    file >> out;
    return true;
  } catch (const std::exception& e) {
    err = e.what();
    return false;
  }
}

static bool write_json_file(const std::string& path, const json& data, std::string& err) {
  try {
    std::filesystem::path p(path);

    if (p.has_parent_path()) {
      std::filesystem::create_directories(p.parent_path());
    }

    const std::string tmp_path = path + ".tmp";

    {
      std::ofstream out(tmp_path, std::ios::binary | std::ios::trunc);
      if (!out.is_open()) {
        err = "failed to write file: " + tmp_path;
        return false;
      }

      out << data.dump(2) << "\n";
    }

    std::filesystem::rename(tmp_path, path);
    return true;
  } catch (const std::exception& e) {
    err = e.what();
    return false;
  }
}

static bool validate_rule_json(const json& rule, std::string& err) {
  if (!rule.is_object()) {
    err = "rule must be a JSON object";
    return false;
  }

  if (!rule.contains("id") || !rule["id"].is_string() || rule["id"].get<std::string>().empty()) {
    err = "rule.id must be a non-empty string";
    return false;
  }

  if (rule.contains("enabled") && !rule["enabled"].is_boolean()) {
    err = "rule.enabled must be boolean";
    return false;
  }

  if (rule.contains("type") && !rule["type"].is_string()) {
    err = "rule.type must be string";
    return false;
  }

  if (rule.contains("threshold") && !rule["threshold"].is_number_integer()) {
    err = "rule.threshold must be integer";
    return false;
  }

  if (rule.contains("window_sec") && !rule["window_sec"].is_number_integer()) {
    err = "rule.window_sec must be integer";
    return false;
  }

  if (rule.contains("suppress_sec") && !rule["suppress_sec"].is_number_integer()) {
    err = "rule.suppress_sec must be integer";
    return false;
  }

  return true;
}

static int find_rule_index_by_id(const json& rules, const std::string& id) {
  if (!rules.is_array()) return -1;

  for (std::size_t i = 0; i < rules.size(); ++i) {
    const auto& item = rules[i];

    if (!item.is_object()) continue;
    if (!item.contains("id") || !item["id"].is_string()) continue;

    if (item["id"].get<std::string>() == id) {
      return static_cast<int>(i);
    }
  }

  return -1;
}

static json default_array_root() {
  return json::array();
}

static bool ensure_array_file(const std::string& path, std::string& err) {
  if (std::filesystem::exists(path)) return true;
  return write_json_file(path, json::array(), err);
}

static int find_json_item_index_by_id(const json& arr, const std::string& id) {
  if (!arr.is_array()) return -1;

  for (std::size_t i = 0; i < arr.size(); ++i) {
    const auto& item = arr[i];

    if (!item.is_object()) continue;
    if (!item.contains("id") || !item["id"].is_string()) continue;

    if (item["id"].get<std::string>() == id) {
      return static_cast<int>(i);
    }
  }

  return -1;
}

static bool json_string_array_contains(const json& arr, const std::string& value) {
  if (!arr.is_array()) return false;

  for (const auto& item : arr) {
    if (item.is_string() && item.get<std::string>() == value) {
      return true;
    }
  }

  return false;
}

static bool validate_receiver_json(const json& receiver, std::string& err) {
  if (!receiver.is_object()) {
    err = "receiver must be JSON object";
    return false;
  }

  if (!receiver.contains("id") || !receiver["id"].is_string() || receiver["id"].get<std::string>().empty()) {
    err = "receiver.id must be non-empty string";
    return false;
  }

  if (!receiver.contains("name") || !receiver["name"].is_string() || receiver["name"].get<std::string>().empty()) {
    err = "receiver.name must be non-empty string";
    return false;
  }

  if (!receiver.contains("type") || !receiver["type"].is_string()) {
    err = "receiver.type must be string";
    return false;
  }

  const std::string type = receiver["type"].get<std::string>();

  if (type != "file" && type != "directory") {
    err = "receiver.type must be file or directory";
    return false;
  }

  const std::string role =
    receiver.contains("role") && receiver["role"].is_string()
      ? receiver["role"].get<std::string>()
      : "receiver";

  if (role != "esm" && role != "erc" && role != "receiver" && role != "device") {
    err = "receiver.role must be esm, erc, receiver or device";
    return false;
  }

  if (receiver.contains("parent_id") && !receiver["parent_id"].is_string()) {
    err = "receiver.parent_id must be string";
    return false;
  }

  if (!receiver.contains("path") || !receiver["path"].is_string()) {
    err = "receiver.path must be string";
    return false;
  }

  if ((role == "receiver" || role == "device") && receiver["path"].get<std::string>().empty()) {
    err = "receiver.path must be non-empty for receiver/device";
    return false;
  }

  if (receiver.contains("dashboard_ids") && !receiver["dashboard_ids"].is_array()) {
    err = "receiver.dashboard_ids must be array";
    return false;
  }

  if (receiver.contains("enabled") && !receiver["enabled"].is_boolean()) {
    err = "receiver.enabled must be boolean";
    return false;
  }

  if (receiver.contains("policy_group_ids") && !receiver["policy_group_ids"].is_array()) {
    err = "receiver.policy_group_ids must be array";
    return false;
  }

  if (receiver.contains("file_pattern") && !receiver["file_pattern"].is_string()) {
    err = "receiver.file_pattern must be string";
    return false;
  }

  if (receiver.contains("recursive") && !receiver["recursive"].is_boolean()) {
    err = "receiver.recursive must be boolean";
    return false;
  }

  return true;
}

static bool validate_policy_group_json(const json& group, std::string& err) {
  if (!group.is_object()) {
    err = "policy group must be JSON object";
    return false;
  }

  if (!group.contains("id") || !group["id"].is_string() || group["id"].get<std::string>().empty()) {
    err = "policy_group.id must be non-empty string";
    return false;
  }

  if (!group.contains("name") || !group["name"].is_string() || group["name"].get<std::string>().empty()) {
    err = "policy_group.name must be non-empty string";
    return false;
  }

  if (group.contains("enabled") && !group["enabled"].is_boolean()) {
    err = "policy_group.enabled must be boolean";
    return false;
  }

  return true;
}

static bool validate_parser_rule_json(const json& rule, std::string& err) {
  if (!rule.is_object()) {
    err = "parser rule must be JSON object";
    return false;
  }

  if (!rule.contains("id") || !rule["id"].is_string() || rule["id"].get<std::string>().empty()) {
    err = "parser_rule.id must be non-empty string";
    return false;
  }

  if (!rule.contains("name") || !rule["name"].is_string() || rule["name"].get<std::string>().empty()) {
    err = "parser_rule.name must be non-empty string";
    return false;
  }

  if (!rule.contains("policy_group_id") || !rule["policy_group_id"].is_string()) {
    err = "parser_rule.policy_group_id must be string";
    return false;
  }

  if (rule.contains("enabled") && !rule["enabled"].is_boolean()) {
    err = "parser_rule.enabled must be boolean";
    return false;
  }

  if (!rule.contains("match_type") || !rule["match_type"].is_string()) {
    err = "parser_rule.match_type must be string";
    return false;
  }

  const std::string match_type = rule["match_type"].get<std::string>();

  if (match_type != "regex" && match_type != "json") {
    err = "parser_rule.match_type must be regex or json";
    return false;
  }

  if (match_type == "regex") {
    if (!rule.contains("pattern") || !rule["pattern"].is_string() || rule["pattern"].get<std::string>().empty()) {
      err = "regex parser_rule.pattern must be non-empty string";
      return false;
    }

    if (!rule.contains("field_order") || !rule["field_order"].is_array()) {
      err = "regex parser_rule.field_order must be array";
      return false;
    }
  }

  if (match_type == "json") {
    if (rule.contains("json_paths") && !rule["json_paths"].is_object()) {
      err = "json parser_rule.json_paths must be object";
      return false;
    }
  }

  if (!rule.contains("field_mapping") || !rule["field_mapping"].is_object()) {
    err = "parser_rule.field_mapping must be object";
    return false;
  }

  if (!rule.contains("constants") || !rule["constants"].is_object()) {
    err = "parser_rule.constants must be object";
    return false;
  }

  if (!rule.contains("enabled_receivers") || !rule["enabled_receivers"].is_array()) {
    err = "parser_rule.enabled_receivers must be array";
    return false;
  }

  return true;
}

static json load_json_array_file_or_empty(const std::string& path) {
  json arr;
  std::string err;

  if (!ensure_array_file(path, err)) {
    return json::array();
  }

  if (!read_json_file(path, arr, err)) {
    return json::array();
  }

  if (!arr.is_array()) {
    return json::array();
  }

  return arr;
}

static json extract_regex_fields(const std::string& raw,
                                 const std::string& pattern,
                                 const json& field_order,
                                 bool& matched,
                                 std::string& err) {
  matched = false;
  json fields = json::object();

  try {
    std::regex re(pattern);
    std::smatch match;

    if (!std::regex_search(raw, match, re)) {
      return fields;
    }

    matched = true;

    for (std::size_t i = 0; i < field_order.size(); ++i) {
      if (!field_order[i].is_string()) continue;

      const std::size_t match_index = i + 1;
      const std::string field_name = field_order[i].get<std::string>();

      if (match_index < match.size()) {
        fields[field_name] = match[match_index].str();
      } else {
        fields[field_name] = "";
      }
    }

    return fields;
  } catch (const std::exception& e) {
    err = e.what();
    return fields;
  }
}

static std::string parser_json_value_to_string(const json& value) {
  if (value.is_null()) return "";
  if (value.is_string()) return value.get<std::string>();
  if (value.is_boolean()) return value.get<bool>() ? "true" : "false";
  if (value.is_number_integer()) return std::to_string(value.get<long long>());
  if (value.is_number_unsigned()) return std::to_string(value.get<unsigned long long>());
  if (value.is_number_float()) return std::to_string(value.get<double>());
  return value.dump();
}

static void flatten_json_fields(const json& value,
                                const std::string& prefix,
                                json& out) {
  if (value.is_object()) {
    for (auto it = value.begin(); it != value.end(); ++it) {
      const std::string key = prefix.empty()
        ? it.key()
        : prefix + "." + it.key();

      flatten_json_fields(it.value(), key, out);
    }

    return;
  }

  if (value.is_array()) {
    out[prefix] = value.dump();
    return;
  }

  if (!prefix.empty()) {
    out[prefix] = parser_json_value_to_string(value);
  }
}

static std::string normalize_json_path(std::string path) {
  if (path.rfind("$.", 0) == 0) {
    path = path.substr(2);
  } else if (path == "$") {
    path.clear();
  }

  return path;
}

static bool get_json_dot_path(const json& root,
                              const std::string& raw_path,
                              json& out) {
  std::string path = normalize_json_path(raw_path);

  if (path.empty()) {
    out = root;
    return true;
  }

  const json* current = &root;
  std::size_t start = 0;

  while (start < path.size()) {
    const std::size_t dot = path.find('.', start);
    const std::string part = path.substr(
      start,
      dot == std::string::npos ? std::string::npos : dot - start
    );

    if (part.empty()) {
      return false;
    }

    if (current->is_object()) {
      if (!current->contains(part)) {
        return false;
      }

      current = &((*current)[part]);
    } else if (current->is_array()) {
      try {
        const std::size_t index = static_cast<std::size_t>(std::stoul(part));

        if (index >= current->size()) {
          return false;
        }

        current = &((*current)[index]);
      } catch (...) {
        return false;
      }
    } else {
      return false;
    }

    if (dot == std::string::npos) {
      out = *current;
      return true;
    }

    start = dot + 1;
  }

  return false;
}

static json extract_json_fields(const std::string& raw,
                                const json& json_paths,
                                bool& matched,
                                std::string& err) {
  matched = false;
  json fields = json::object();

  try {
    json root = json::parse(raw);

    if (!root.is_object()) {
      err = "raw JSON must be an object";
      return fields;
    }

    matched = true;

    if (json_paths.is_object() && !json_paths.empty()) {
      for (auto it = json_paths.begin(); it != json_paths.end(); ++it) {
        const std::string output_name = it.key();

        if (!it.value().is_string()) {
          continue;
        }

        const std::string path = it.value().get<std::string>();

        json value;
        if (get_json_dot_path(root, path, value)) {
          fields[output_name] = parser_json_value_to_string(value);
        } else {
          fields[output_name] = "";
        }
      }

      return fields;
    }

    flatten_json_fields(root, "", fields);
    return fields;
  } catch (const std::exception& e) {
    err = e.what();
    return fields;
  }
}

static std::string resolve_mapping_value(const std::string& value, const json& extracted) {
  if (value.size() >= 2 && value[0] == '$') {
    const std::string key = value.substr(1);

    if (extracted.contains(key)) {
      if (extracted[key].is_string()) return extracted[key].get<std::string>();
      return extracted[key].dump();
    }

    return "";
  }

  return value;
}

static void set_alias_field(json& event, const std::string& target_field, const std::string& value) {
  if (value.empty()) return;

  // Always keep direct flat field too.
  // This lets rules use both "source.ip" and aliases like "src_ip".
  event[target_field] = value;

  if (target_field == "user.name" ||
      target_field == "source.user.name" ||
      target_field == "destination.user.name" ||
      target_field == "account.name" ||
      target_field == "actor.name") {
    event["user"] = value;
  }

  if (target_field == "source.ip" ||
      target_field == "client.ip" ||
      target_field == "observer.ip") {
    event["src_ip"] = value;
    event["ip"] = value;
  }

  if (target_field == "destination.ip" ||
      target_field == "server.ip") {
    event["dst_ip"] = value;
  }

  if (target_field == "source.port" ||
      target_field == "client.port") {
    event["src_port"] = value;
    event["port"] = value;
  }

  if (target_field == "destination.port" ||
      target_field == "server.port") {
    event["dst_port"] = value;
  }

  if (target_field == "process.name" ||
      target_field == "process.executable" ||
      target_field == "syslog.identifier" ||
      target_field == "program") {
    event["process_name"] = value;
    event["program"] = value;
  }

  if (target_field == "process.pid" ||
      target_field == "syslog.pid") {
    event["pid"] = value;
  }

  if (target_field == "process.command_line" ||
      target_field == "auth.sudo.command") {
    event["command"] = value;
    event["cmdline"] = value;
  }

  if (target_field == "file.path" ||
      target_field == "file.target_path" ||
      target_field == "registry.path") {
    event["path"] = value;
  }

  if (target_field == "network.protocol" ||
      target_field == "auth.protocol") {
    event["protocol"] = value;
  }

  if (target_field == "event.name") {
    event["event_name"] = value;
  }

  if (target_field == "event.code") {
    event["event_code"] = value;
  }

  if (target_field == "event.type") {
    event["event_type"] = value;
  }

  if (target_field == "event.category") {
    event["event_category"] = value;
  }

  if (target_field == "event.action") {
    event["event_action"] = value;
  }

  if (target_field == "event.outcome") {
    event["event_outcome"] = value;
  }

  if (target_field == "event.severity") {
    event["severity"] = value;
  }

  if (target_field == "host.name" ||
      target_field == "host.hostname") {
    event["host"] = value;
  }

  if (target_field == "service.name" ||
      target_field == "pam.service") {
    event["service"] = value;
  }

  if (target_field == "linux.kernel.error_code" ||
      target_field == "event.error_code") {
    event["error_code"] = value;
  }

  if (target_field == "url.original") {
    event["url"] = value;
  }

  if (target_field == "dns.question.name") {
    event["domain"] = value;
  }

  if (target_field == "threat.name" ||
      target_field == "malware.name") {
    event["threat_name"] = value;
  }
}

static bool apply_parser_rules(json& event, const std::string& parser_rules_path) {
  const std::string raw = safe_string(event, "raw", "");
  const std::string receiver_id = safe_string(event, "receiver_id", "");

  event["parser_status"] = "unparsed";

  if (safe_string(event, "event_type", "").empty()) {
    event["event_type"] = "unknown_event";
  }

  if (safe_string(event, "severity", "").empty()) {
    event["severity"] = "info";
  }

  if (raw.empty() || receiver_id.empty()) {
    return false;
  }

  const json rules = load_json_array_file_or_empty(parser_rules_path);

  for (const auto& rule : rules) {
    if (!rule.is_object()) continue;
    if (rule.value("enabled", true) == false) continue;

    if (!rule.contains("enabled_receivers") ||
        !json_string_array_contains(rule["enabled_receivers"], receiver_id)) {
      continue;
    }

    const std::string match_type = safe_string(rule, "match_type", "regex");

    bool matched = false;
    json extracted = json::object();
    std::string err;

    if (match_type == "regex") {
      extracted = extract_regex_fields(
        raw,
        safe_string(rule, "pattern", ""),
        rule.value("field_order", json::array()),
        matched,
        err
      );
    } else if (match_type == "json") {
        extracted = extract_json_fields(
        raw,
        rule.value("json_paths", json::object()),
        matched,
        err
      );
    }

    if (!matched) {
      continue;
    }

    json fields = json::object();

    const json mapping = rule.value("field_mapping", json::object());
    for (auto it = mapping.begin(); it != mapping.end(); ++it) {
      const std::string target_field = it.key();
      const std::string source_expr = it.value().is_string() ? it.value().get<std::string>() : it.value().dump();
      const std::string mapped_value = resolve_mapping_value(source_expr, extracted);

      fields[target_field] = mapped_value;
      set_alias_field(event, target_field, mapped_value);
    }

    const json constants = rule.value("constants", json::object());
    for (auto it = constants.begin(); it != constants.end(); ++it) {
      event[it.key()] = it.value();
    }

    event["fields"] = fields;
    event["extracted"] = extracted;
    event["parser_status"] = "parsed";
    event["parser_rule_id"] = safe_string(rule, "id", "");
    event["parser_rule_name"] = safe_string(rule, "name", "");
    event["policy_group_id"] = safe_string(rule, "policy_group_id", "");

    return true;
  }

  event["event_type"] = "unknown_event";
  event["event_name"] = "Unknown Event";
  event["severity"] = "info";
  event["fields"] = json::object();

  return false;
}

static std::string make_dashboard_id(const std::string& title) {
  std::string out = "DASHBOARD_";

  for (char c : title) {
    if (c >= 'a' && c <= 'z') {
      out.push_back(static_cast<char>(c - 'a' + 'A'));
    } else if (c >= 'A' && c <= 'Z') {
      out.push_back(c);
    } else if (c >= '0' && c <= '9') {
      out.push_back(c);
    } else if (c == '-' || c == '_' || c == ' ') {
      if (!out.empty() && out.back() != '_') {
        out.push_back('_');
      }
    }
  }

  while (!out.empty() && out.back() == '_') {
    out.pop_back();
  }

  if (out == "DASHBOARD") {
    out = "DASHBOARD_CUSTOM";
  }

  out += "_" + std::to_string(
    std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::system_clock::now().time_since_epoch()
    ).count()
  );

  return out;
}

static json default_dashboards_root() {
  return {
    {"version", 1},
    {"dashboards", json::array()}
  };
}

static bool ensure_dashboards_file(const std::string& path, std::string& err) {
  if (std::filesystem::exists(path)) {
    return true;
  }

  return write_json_file(path, default_dashboards_root(), err);
}

static bool normalize_dashboards_root(json& root, std::string& err) {
  if (root.is_array()) {
    root = {
      {"version", 1},
      {"dashboards", root}
    };
    return true;
  }

  if (!root.is_object()) {
    err = "dashboards file must contain JSON object";
    return false;
  }

  if (!root.contains("version")) {
    root["version"] = 1;
  }

  if (!root.contains("dashboards")) {
    root["dashboards"] = json::array();
  }

  if (!root["dashboards"].is_array()) {
    err = "dashboards field must be JSON array";
    return false;
  }

  return true;
}

static bool validate_dashboard_json(const json& dashboard, std::string& err) {
  if (!dashboard.is_object()) {
    err = "dashboard must be a JSON object";
    return false;
  }

  if (!dashboard.contains("id") || !dashboard["id"].is_string() || dashboard["id"].get<std::string>().empty()) {
    err = "dashboard.id must be a non-empty string";
    return false;
  }

  if (!dashboard.contains("title") || !dashboard["title"].is_string() || dashboard["title"].get<std::string>().empty()) {
    err = "dashboard.title must be a non-empty string";
    return false;
  }

  if (dashboard.contains("description") && !dashboard["description"].is_string()) {
    err = "dashboard.description must be string";
    return false;
  }

  if (dashboard.contains("locked") && !dashboard["locked"].is_boolean()) {
    err = "dashboard.locked must be boolean";
    return false;
  }

  if (dashboard.contains("layout") && !dashboard["layout"].is_object()) {
    err = "dashboard.layout must be object";
    return false;
  }

  if (!dashboard.contains("widgets")) {
    err = "dashboard.widgets is required";
    return false;
  }

  if (!dashboard["widgets"].is_array()) {
    err = "dashboard.widgets must be array";
    return false;
  }

  for (std::size_t i = 0; i < dashboard["widgets"].size(); ++i) {
    const auto& widget = dashboard["widgets"][i];

    if (!widget.is_object()) {
      err = "dashboard.widgets[" + std::to_string(i) + "] must be object";
      return false;
    }

    if (!widget.contains("id") || !widget["id"].is_string() || widget["id"].get<std::string>().empty()) {
      err = "dashboard.widgets[" + std::to_string(i) + "].id must be non-empty string";
      return false;
    }

    if (!widget.contains("type") || !widget["type"].is_string() || widget["type"].get<std::string>().empty()) {
      err = "dashboard.widgets[" + std::to_string(i) + "].type must be non-empty string";
      return false;
    }

    if (widget.contains("title") && !widget["title"].is_string()) {
      err = "dashboard.widgets[" + std::to_string(i) + "].title must be string";
      return false;
    }

    if (widget.contains("width") && !widget["width"].is_string()) {
      err = "dashboard.widgets[" + std::to_string(i) + "].width must be string";
      return false;
    }

    if (widget.contains("limit") && !widget["limit"].is_number_integer()) {
      err = "dashboard.widgets[" + std::to_string(i) + "].limit must be integer";
      return false;
    }

    if (widget.contains("filters") && !widget["filters"].is_object()) {
      err = "dashboard.widgets[" + std::to_string(i) + "].filters must be object";
      return false;
    }
  }

  return true;
}

static int find_dashboard_index_by_id(const json& root, const std::string& id) {
  if (!root.is_object()) return -1;
  if (!root.contains("dashboards") || !root["dashboards"].is_array()) return -1;

  const auto& dashboards = root["dashboards"];

  for (std::size_t i = 0; i < dashboards.size(); ++i) {
    const auto& item = dashboards[i];

    if (!item.is_object()) continue;
    if (!item.contains("id") || !item["id"].is_string()) continue;

    if (item["id"].get<std::string>() == id) {
      return static_cast<int>(i);
    }
  }

  return -1;
}

static void send_json_response(httplib::Response& res, int status, const json& body) {
  res.status = status;
  res.set_content(body.dump(), "application/json");
}

static json make_event_response_item(long long id, const json& original) {
  return json {
    {"id", id},
    {"event_id", safe_string(original, "event_id", "")},
    {"ts", safe_string(original, "ts", "")},
    {"received_at", safe_string(original, "received_at", "")},
    {"host", safe_string(original, "host", "unknown")},
    {"event_type", safe_string(original, "event_type", "unknown")},
    {"source", safe_string(original, "source", "unknown")},
    {"source_type", safe_string(original, "source_type", "syslog")},
    {"severity", safe_string(original, "severity", "info")},
    {"event", original}
  };
}

static json make_alert_response_item(const json& alert) {
  return json {
    {"id", 0},
    {"ts", safe_string(alert, "ts", "")},
    {"rule_name", safe_string(alert, "rule_name", "")},
    {"severity", safe_string(alert, "severity", "")},
    {"title", safe_string(alert, "title", "Alert")},
    {"description", safe_string(alert, "description", "")},
    {"alert", alert}
  };
}

static std::string sse_message(const std::string& event_name, const json& payload){
  return "event: " + event_name + "\n" +
	 "data: " + payload.dump() + "\n\n";
}

static int clamp_limit(int requested, int def, int max_value) {
  if (requested <= 0) return def;

  // max_value <= 0 means unlimited
  if (max_value > 0 && requested > max_value) {
    return max_value;
  }

  return requested;
}
static std::string get_env_or_default(const char* name, const std::string& def) {
  const char* value = std::getenv(name);
  if (!value || std::string(value).empty()) return def;
  return std::string(value);
}

static std::string trim_left_copy(std::string s) {
  while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) {
    s.erase(s.begin());
  }
  return s;
}

static std::string get_cookie_value(const httplib::Request& req, const std::string& name) {
  if (!req.has_header("Cookie")) return "";

  const std::string cookie = req.get_header_value("Cookie");
  const std::string prefix = name + "=";

  std::size_t start = 0;

  while (start < cookie.size()) {
    std::size_t end = cookie.find(';', start);
    std::string part = cookie.substr(start, end == std::string::npos ? std::string::npos : end - start);
    part = trim_left_copy(part);

    if (part.rfind(prefix, 0) == 0) {
      return part.substr(prefix.size());
    }

    if (end == std::string::npos) break;
    start = end + 1;
  }

  return "";
}

static void set_auth_cookie(httplib::Response& res,
                            const std::string& cookie_name,
                            const std::string& session_id,
                            int ttl_seconds) {
  res.set_header(
    "Set-Cookie",
    cookie_name + "=" + session_id +
    "; HttpOnly; SameSite=Lax; Path=/; Max-Age=" +
    std::to_string(ttl_seconds)
  );
}

static void clear_auth_cookie(httplib::Response& res, const std::string& cookie_name) {
  res.set_header(
    "Set-Cookie",
    cookie_name + "=; HttpOnly; SameSite=Lax; Path=/; Max-Age=0"
  );
}

static int role_level(const std::string& role) {
  if (role == "admin") return 3;
  if (role == "analyst") return 2;
  if (role == "viewer") return 1;
  return 0;
}

struct SseClient {
  std::mutex mutex;
  std::deque<std::string> queue;
  bool active = true;
};

int main() {
  try {
    // Важно: относительный путь => работает с флешки
    const AppConfig config = ConfigLoader::load_from_file("config/config.json");

    std::filesystem::create_directories(config.paths.data_dir);
    std::filesystem::create_directories(config.paths.logs_dir);

    SqliteDb db(config.server.db_path);
    db.init();
    DetectionEngine detector(db, config.detection);
    CorrelationEngine correlator(db);

    AuthDb auth_db(config.auth.db_path);
    auth_db.init();

    SessionStore session_store(config.auth.db_path, config.auth.session_ttl_seconds);
    session_store.init();

    if (!auth_db.has_users()) {
      const std::string admin_user =
        get_env_or_default("MINI_SIEM_ADMIN_USER", "admin");
      const std::string admin_password =
        get_env_or_default("MINI_SIEM_ADMIN_PASSWORD", "admin123");

      std::string err;
      if (auth_db.create_user(admin_user, admin_password, "admin", true, false, err)) {
        std::cerr << "[auth] created initial admin user: " << admin_user << "\n";
      } else {
        std::cerr << "[auth][ERR] failed to create initial admin: " << err << "\n";
      }
    }

    const std::string agent_token =
      get_env_or_default("MINI_SIEM_AGENT_TOKEN", "dev-agent-token");


    httplib::Server srv;

    std::mutex clients_mutex;
    std::vector<std::shared_ptr<SseClient>> sse_clients;

    auto broadcast_sse = [&](const std::string& event_name, const json& payload) {
      const std::string msg = sse_message(event_name, payload);

      std::lock_guard<std::mutex> lock(clients_mutex);
      for (auto& client : sse_clients) {
	if (!client) continue;
	std::lock_guard<std::mutex> client_lock(client->mutex);
	if (client->active) {
	  client->queue.push_back(msg);
	}
      }
    };

    auto read_session = [&](const httplib::Request& req, AuthSession& session) {
      const std::string session_id = get_cookie_value(req, config.auth.session_cookie);
      if (session_id.empty()) return false;
      return session_store.find_session(session_id, session);
    };

    auto require_login = [&](const httplib::Request& req,
                             httplib::Response& res,
                             AuthSession& session) {
      if (read_session(req, session)) {
        return true;
      }

      send_json_response(res, 401, {
        {"status", "error"},
        {"message", "login required"}
      });
      return false;
    };

    auto require_role = [&](const httplib::Request& req,
                            httplib::Response& res,
                            const std::string& min_role,
                            AuthSession& session) {
      if (!require_login(req, res, session)) {
        return false;
      }

      if (role_level(session.role) < role_level(min_role)) {
      	  send_json_response(res, 403, {
          {"status", "error"},
          {"message", "permission denied"}
        });
        return false;
      }

      return true;
    };

    auto require_agent_token = [&](const httplib::Request& req,
                                   httplib::Response& res) {
      const std::string expected = "Bearer " + agent_token;

      if (!req.has_header("Authorization") ||
          req.get_header_value("Authorization") != expected) {
        send_json_response(res, 401, {
          {"status", "error"},
          {"message", "agent token required"}
        });
        return false;
      }

      return true;
    };

    // Healthcheck
    srv.Get("/health", [&](const httplib::Request&, httplib::Response& res) {
      res.set_content("OK\n", "text/plain");
    });

    // Auth API
    srv.Post("/api/auth/login", [&](const httplib::Request& req, httplib::Response& res) {
      try {
        json body = json::parse(req.body);

        const std::string username = safe_string(body, "username", "");
        const std::string password = safe_string(body, "password", "");

        AuthUser user;
        std::string err;

        if (!auth_db.verify_user(username, password, user, err)) {
          send_json_response(res, 401, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        const std::string session_id = session_store.create_session(user.username, user.role);
        set_auth_cookie(res, config.auth.session_cookie, session_id, config.auth.session_ttl_seconds);

        send_json_response(res, 200, {
          {"status", "ok"},
          {"user", {
            {"username", user.username},
            {"role", user.role},
            {"enabled", user.enabled},
  	    {"password_change_required", user.password_change_required}
          }}
        });
      } catch (const std::exception& e) {
        send_json_response(res, 400, {
          {"status", "error"},
          {"message", e.what()}
        });
      }
    });

    srv.Post("/api/auth/logout", [&](const httplib::Request& req, httplib::Response& res) {
      const std::string session_id = get_cookie_value(req, config.auth.session_cookie);

      if (!session_id.empty()) {
        session_store.delete_session(session_id);
      }

      clear_auth_cookie(res, config.auth.session_cookie);

      send_json_response(res, 200, {
        {"status", "ok"}
      });
    });

    srv.Get("/api/auth/me", [&](const httplib::Request& req, httplib::Response& res) {
      AuthSession session;
      if (!require_login(req, res, session)) return;

      bool password_change_required = false;
      bool enabled = true;

      try {
        for (const auto& u : auth_db.get_users()) {
          if (u.username == session.username) {
            password_change_required = u.password_change_required;
            enabled = u.enabled;
            break;
          }
        }
      } catch (...) {
      // keep session data if user listing fails
      }

      if (!enabled) {
        session_store.delete_session(session.id);
        clear_auth_cookie(res, config.auth.session_cookie);

        send_json_response(res, 401, {
          {"status", "error"},
          {"message", "user is disabled"}
        });
        return;
      }

      send_json_response(res, 200, {
        {"status", "ok"},
          {"user", {
     	   {"username", session.username},
     	   {"role", session.role},
     	   {"enabled", enabled},
     	   {"password_change_required", password_change_required}
          }}
        });
      });

srv.Post("/api/auth/change-password", [&](const httplib::Request& req, httplib::Response& res) {
  AuthSession session;
  if (!require_login(req, res, session)) return;

  try {
    json body = json::parse(req.body);

    const std::string old_password = safe_string(body, "old_password", "");
    const std::string new_password = safe_string(body, "new_password", "");

    std::string err;
    if (!auth_db.change_password(session.username, old_password, new_password, err)) {
      send_json_response(res, 400, {
        {"status", "error"},
        {"message", err}
      });
      return;
    }

    send_json_response(res, 200, {
      {"status", "ok"},
      {"action", "password_changed"}
    });
  } catch (const std::exception& e) {
    send_json_response(res, 400, {
      {"status", "error"},
      {"message", e.what()}
    });
  }
});

    // Users API
    srv.Get("/api/users", [&](const httplib::Request& req, httplib::Response& res) {
      AuthSession session;
      if (!require_role(req, res, "admin", session)) return;

      try {
        json arr = json::array();

        for (const auto& u : auth_db.get_users()) {
          arr.push_back({
            {"id", u.id},
            {"username", u.username},
            {"role", u.role},
            {"enabled", u.enabled},
	    {"password_change_required", u.password_change_required},
            {"created_at", u.created_at},
            {"updated_at", u.updated_at}
          });
        }

        send_json_response(res, 200, {
          {"status", "ok"},
          {"users", arr}
        });
      } catch (const std::exception& e) {
        send_json_response(res, 500, {
          {"status", "error"},
          {"message", e.what()}
        });
      }
    });

    srv.Post("/api/users", [&](const httplib::Request& req, httplib::Response& res) {
      AuthSession session;
      if (!require_role(req, res, "admin", session)) return;

      try {
        json body = json::parse(req.body);

        const std::string username = safe_string(body, "username", "");
        const std::string password = safe_string(body, "password", "");
        const std::string role = safe_string(body, "role", "viewer");
        const bool enabled = body.value("enabled", true);

        std::string err;
	if (!auth_db.create_user(username, password, role, enabled, true, err)) {
          send_json_response(res, 400, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        send_json_response(res, 201, {
          {"status", "ok"},
          {"action", "created"},
          {"username", username}
        });
      } catch (const std::exception& e) {
        send_json_response(res, 400, {
          {"status", "error"},
          {"message", e.what()}
        });
      }
    });

    srv.Put(R"(/api/users/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
      AuthSession session;
      if (!require_role(req, res, "admin", session)) return;

      try {
        const std::string username = req.matches[1].str();

        json body = json::parse(req.body);

        const std::string role = safe_string(body, "role", "viewer");
        const bool enabled = body.value("enabled", true);

        std::string err;
        if (!auth_db.update_user(username, role, enabled, err)) {
          send_json_response(res, 400, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        send_json_response(res, 200, {
          {"status", "ok"},
          {"action", "updated"},
          {"username", username}
        });
      } catch (const std::exception& e) {
        send_json_response(res, 400, {
          {"status", "error"},
          {"message", e.what()}
        });
      }
    });

    srv.Delete(R"(/api/users/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
      AuthSession session;
      if (!require_role(req, res, "admin", session)) return;

      const std::string username = req.matches[1].str();

      if (username == session.username) {
        send_json_response(res, 409, {
          {"status", "error"},
          {"message", "cannot delete current user"}
        });
        return;
      }

      std::string err;
      if (!auth_db.delete_user(username, err)) {
        send_json_response(res, 400, {
          {"status", "error"},
          {"message", err}
        });
        return;
      }

      send_json_response(res, 200, {
        {"status", "ok"},
        {"action", "deleted"},
        {"username", username}
      });
    });

    srv.Post(R"(/api/users/([^/]+)/password)", [&](const httplib::Request& req, httplib::Response& res) {
      AuthSession session;
      if (!require_role(req, res, "admin", session)) return;

      try {
        const std::string username = req.matches[1].str();

        json body = json::parse(req.body);
        const std::string password = safe_string(body, "password", "");

        std::string err;
	if (!auth_db.update_password(username, password, true, err)) {
          send_json_response(res, 400, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        send_json_response(res, 200, {
          {"status", "ok"},
          {"action", "password_updated"},
          {"username", username}
        });
      } catch (const std::exception& e) {
        send_json_response(res, 400, {
          {"status", "error"},
          {"message", e.what()}
        });
      }
    });

    // Static: index
    srv.Get("/", [&](const httplib::Request&, httplib::Response& res) {
      std::string content;
      if (!read_file("web/index.html", content)) {
	res.status = 404;
	res.set_content("web/index.html not found\n", "text/plain");
	return;
      }
      res.set_content(content, "text/html; charset=UTF-8");
    });


    // Static: JS
    srv.Get("/app.js", [&](const httplib::Request&, httplib::Response& res){
      std::string content;
      if (!read_file("web/app.js", content)){
	res.status = 404;
	res.set_content("web/app.js not found\n", "text/plain");
        return;
      }
      res.set_content(content, "application/javascript; charset=UTF-8");
    });

    // Static: CSS
    srv.Get("/style.css", [&](const httplib::Request&, httplib::Response& res) {
      std::string content;
      if (!read_file("web/style.css", content)) {
	res.status = 404;
	res.set_content("web/style.css not found\n", "text/plain");
	return;
      }
      res.set_content(content, "text/css; charset=UTF-8");
    });

    srv.Get("/stream", [&](const httplib::Request& req, httplib::Response& res){
      AuthSession session;
      if (!require_login(req, res, session)) return;

      res.set_header("Cache-Control", "no-cache");
      res.set_header("Connection", "keep-alive");
      res.set_header("X-Accel-Buffering", "no");

      auto client = std::make_shared<SseClient>();

      {
	std::lock_guard<std::mutex> lock(clients_mutex);
	sse_clients.push_back(client);
      }

      res.set_chunked_content_provider(
	"text/event-stream",
	[&, client](size_t, httplib::DataSink& sink) {
	  {
	    std::lock_guard<std::mutex> lock(clients_mutex);
	    client->queue.push_back("event: hello\ndata: {\"status\":\"connected\"}\n\n");
	  }

	  while (sink.is_writable()) {
	    std::string next_message;

	    {
	      std::lock_guard<std::mutex> lock(client->mutex);
	      if (!client->queue.empty()) {
		next_message = std::move(client->queue.front());
		client->queue.pop_front();
	      }
	    }
	    if (!next_message.empty()) {
	      if (!sink.write(next_message.c_str(), next_message.size())) {
		break;
	      }
	    } else {
	      const std::string heartbeat = ": ping\n\n";
	      if (!sink.write(heartbeat.c_str(), heartbeat.size())) {
		break;
	      }
	      std::this_thread::sleep_for(std::chrono::seconds(2));
	    }
	  }
	  {
	    std::lock_guard<std::mutex> lock(client->mutex);
	    client->active = false;
	  }
	  {
	    std::lock_guard<std::mutex> lock(clients_mutex);
	    auto it = std::remove_if(
	      sse_clients.begin(),
	      sse_clients.end(),
	      [&](const std::shared_ptr<SseClient>& c) {
		return !c || c == client;
	      }
	    );
	    sse_clients.erase(it, sse_clients.end());
	  }

	  sink.done();
	  return false;
	}
      );
    });
    const std::string rules_path = "config/rules.json";
    std::mutex rules_file_mutex;

    const std::string receivers_path = "config/receivers.json";
    const std::string policy_groups_path = "config/policy_groups.json";
    const std::string parser_rules_path = "config/parser_rules.json";
    const std::string field_catalog_path = "config/field_catalog.json";

    std::mutex receivers_file_mutex;
    std::mutex policy_groups_file_mutex;
    std::mutex parser_rules_file_mutex;

    auto handle_get_array_file = [&](const httplib::Request& req,
                                     httplib::Response& res,
                                     const std::string& path,
                                     std::mutex& file_mutex,
                                     const std::string& response_key) {
      AuthSession session;
      if (!require_role(req, res, "analyst", session)) return;

      std::lock_guard<std::mutex> lock(file_mutex);

      json arr;
      std::string err;

      if (!ensure_array_file(path, err)) {
        send_json_response(res, 500, {{"status", "error"}, {"message", err}});
        return;
      }

      if (!read_json_file(path, arr, err)) {
        send_json_response(res, 500, {{"status", "error"}, {"message", err}});
        return;
      }

      if (!arr.is_array()) {
        send_json_response(res, 500, {{"status", "error"}, {"message", path + " must contain JSON array"}});
        return;
      }

      send_json_response(res, 200, {
        {"status", "ok"},
        {"count", arr.size()},
        {response_key, arr}
      });
    };

    auto handle_create_array_item = [&](const httplib::Request& req,
                                        httplib::Response& res,
                                        const std::string& path,
                                        std::mutex& file_mutex,
                                        const std::string& response_key,
                                        auto validator) {
      AuthSession session;
      if (!require_role(req, res, "admin", session)) return;

      std::lock_guard<std::mutex> lock(file_mutex);

      try {
        json item = json::parse(req.body);

        std::string err;
        if (!validator(item, err)) {
          send_json_response(res, 400, {{"status", "error"}, {"message", err}});
          return;
        }

        json arr;

        if (!ensure_array_file(path, err)) {
          send_json_response(res, 500, {{"status", "error"}, {"message", err}});
          return;
        }

        if (!read_json_file(path, arr, err)) {
          send_json_response(res, 500, {{"status", "error"}, {"message", err}});
          return;
        }

        if (!arr.is_array()) {
          send_json_response(res, 500, {{"status", "error"}, {"message", path + " must contain JSON array"}});
          return;
        }

        const std::string id = item["id"].get<std::string>();

        if (find_json_item_index_by_id(arr, id) >= 0) {
          send_json_response(res, 409, {
            {"status", "error"},
            {"message", "item already exists"},
            {"id", id}
          });
          return;
        }

        arr.push_back(item);

        if (!write_json_file(path, arr, err)) {
          send_json_response(res, 500, {{"status", "error"}, {"message", err}});
          return;
        }

        send_json_response(res, 201, {
          {"status", "ok"},
          {"action", "created"},
          {"id", id},
          {response_key, item}
        });
      } catch (const std::exception& e) {
        send_json_response(res, 400, {{"status", "error"}, {"message", e.what()}});
      }
    };

    auto handle_update_array_item = [&](const httplib::Request& req,
                                        httplib::Response& res,
                                        const std::string& path,
                                        std::mutex& file_mutex,
                                        const std::string& response_key,
                                        auto validator) {
      AuthSession session;
      if (!require_role(req, res, "admin", session)) return;

      std::lock_guard<std::mutex> lock(file_mutex);

      try {
        const std::string id = req.matches[1].str();

        json item = json::parse(req.body);
        item["id"] = id;

        std::string err;
        if (!validator(item, err)) {
          send_json_response(res, 400, {{"status", "error"}, {"message", err}});
          return;
        }

        json arr;

        if (!read_json_file(path, arr, err)) {
          send_json_response(res, 500, {{"status", "error"}, {"message", err}});
          return;
        }

        if (!arr.is_array()) {
          send_json_response(res, 500, {{"status", "error"}, {"message", path + " must contain JSON array"}});
          return;
        }

        const int index = find_json_item_index_by_id(arr, id);

        if (index < 0) {
          send_json_response(res, 404, {
            {"status", "error"},
            {"message", "item not found"},
            {"id", id}
          });
          return;
        }

        arr[static_cast<std::size_t>(index)] = item;

        if (!write_json_file(path, arr, err)) {
          send_json_response(res, 500, {{"status", "error"}, {"message", err}});
          return;
        }

        send_json_response(res, 200, {
          {"status", "ok"},
          {"action", "updated"},
          {"id", id},
          {response_key, item}
        });
      } catch (const std::exception& e) {
        send_json_response(res, 400, {{"status", "error"}, {"message", e.what()}});
      }
    };

    auto handle_delete_array_item = [&](const httplib::Request& req,
                                        httplib::Response& res,
                                        const std::string& path,
                                        std::mutex& file_mutex,
                                        const std::string& response_key) {
      AuthSession session;
      if (!require_role(req, res, "admin", session)) return;

      std::lock_guard<std::mutex> lock(file_mutex);

      try {
        const std::string id = req.matches[1].str();

        json arr;
        std::string err;

        if (!read_json_file(path, arr, err)) {
          send_json_response(res, 500, {{"status", "error"}, {"message", err}});
          return;
        }

        if (!arr.is_array()) {
          send_json_response(res, 500, {{"status", "error"}, {"message", path + " must contain JSON array"}});
          return;
        }

        const int index = find_json_item_index_by_id(arr, id);

        if (index < 0) {
          send_json_response(res, 404, {
            {"status", "error"},
            {"message", "item not found"},
            {"id", id}
          });
          return;
        }

        json removed = arr[static_cast<std::size_t>(index)];
        arr.erase(arr.begin() + index);

        if (!write_json_file(path, arr, err)) {
          send_json_response(res, 500, {{"status", "error"}, {"message", err}});
          return;
        }

        send_json_response(res, 200, {
          {"status", "ok"},
          {"action", "deleted"},
          {"id", id},
          {response_key, removed}
        });
      } catch (const std::exception& e) {
        send_json_response(res, 400, {{"status", "error"}, {"message", e.what()}});
      }
    };

    // Field Catalog API
    srv.Get("/api/field-catalog", [&](const httplib::Request& req, httplib::Response& res) {
      AuthSession session;
      if (!require_role(req, res, "analyst", session)) return;

      json catalog;
      std::string err;

      if (!read_json_file(field_catalog_path, catalog, err)) {
        send_json_response(res, 500, {
          {"status", "error"},
          {"message", err}
        });
        return;
      }

      if (!catalog.is_object()) {
        send_json_response(res, 500, {
          {"status", "error"},
          {"message", "field catalog must be JSON object"}
        });
        return;
      }

      if (!catalog.contains("fields") || !catalog["fields"].is_array()) {
        send_json_response(res, 500, {
          {"status", "error"},
          {"message", "field catalog must contain fields array"}
        });
        return;
      }

      send_json_response(res, 200, {
        {"status", "ok"},
        {"catalog", catalog}
      });
    });

    // Receivers API
    srv.Get("/api/receivers", [&](const httplib::Request& req, httplib::Response& res) {
      handle_get_array_file(req, res, receivers_path, receivers_file_mutex, "receivers");
    });

    srv.Post("/api/receivers", [&](const httplib::Request& req, httplib::Response& res) {
      handle_create_array_item(req, res, receivers_path, receivers_file_mutex, "receiver", validate_receiver_json);
    });

    srv.Put(R"(/api/receivers/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
      handle_update_array_item(req, res, receivers_path, receivers_file_mutex, "receiver", validate_receiver_json);
    });

    srv.Delete(R"(/api/receivers/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
      handle_delete_array_item(req, res, receivers_path, receivers_file_mutex, "receiver");
    });

    // Policy Groups API
    srv.Get("/api/policy-groups", [&](const httplib::Request& req, httplib::Response& res) {
      handle_get_array_file(req, res, policy_groups_path, policy_groups_file_mutex, "policy_groups");
    });

    srv.Post("/api/policy-groups", [&](const httplib::Request& req, httplib::Response& res) {
      handle_create_array_item(req, res, policy_groups_path, policy_groups_file_mutex, "policy_group", validate_policy_group_json);
    });

    srv.Put(R"(/api/policy-groups/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
      handle_update_array_item(req, res, policy_groups_path, policy_groups_file_mutex, "policy_group", validate_policy_group_json);
    });

    srv.Delete(R"(/api/policy-groups/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
      handle_delete_array_item(req, res, policy_groups_path, policy_groups_file_mutex, "policy_group");
    });

    // Parser Rules API
    srv.Get("/api/parser-rules", [&](const httplib::Request& req, httplib::Response& res) {
      handle_get_array_file(req, res, parser_rules_path, parser_rules_file_mutex, "parser_rules");
    });

    srv.Post("/api/parser-rules", [&](const httplib::Request& req, httplib::Response& res) {
      handle_create_array_item(req, res, parser_rules_path, parser_rules_file_mutex, "parser_rule", validate_parser_rule_json);
    });

    srv.Put(R"(/api/parser-rules/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
      handle_update_array_item(req, res, parser_rules_path, parser_rules_file_mutex, "parser_rule", validate_parser_rule_json);
    });

    srv.Delete(R"(/api/parser-rules/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
      handle_delete_array_item(req, res, parser_rules_path, parser_rules_file_mutex, "parser_rule");
    });

    srv.Post("/api/parser-rules/test", [&](const httplib::Request& req, httplib::Response& res) {
      AuthSession session;
      if (!require_role(req, res, "admin", session)) return;

      try {
        json body = json::parse(req.body);

        json rule = body.value("rule", json::object());
        std::string raw = safe_string(body, "raw", "");

        if (raw.empty()) {
          raw = safe_string(rule, "sample_raw", "");
        }

        std::string err;
        if (!validate_parser_rule_json(rule, err)) {
          send_json_response(res, 400, {{"status", "error"}, {"message", err}});
          return;
        }

        bool matched = false;
        json extracted = json::object();

        const std::string test_match_type = safe_string(rule, "match_type", "regex");

        if (test_match_type == "regex") {
          extracted = extract_regex_fields(
            raw,
   	    safe_string(rule, "pattern", ""),
   	    rule.value("field_order", json::array()),
   	    matched,
  	    err
 	   );
	} else if (test_match_type == "json") {
  	  extracted = extract_json_fields(
  	    raw,
 	    rule.value("json_paths", json::object()),
	    matched,
   	    err
  	  );
	}

        json preview_event = {
          {"raw", raw},
          {"receiver_id", "__test__"},
          {"event_type", "unknown_event"},
          {"parser_status", "unparsed"},
          {"severity", "info"}
        };

        if (matched) {
          json fields = json::object();

          const json mapping = rule.value("field_mapping", json::object());
          for (auto it = mapping.begin(); it != mapping.end(); ++it) {
            const std::string target_field = it.key();
            const std::string source_expr = it.value().is_string() ? it.value().get<std::string>() : it.value().dump();
            const std::string mapped_value = resolve_mapping_value(source_expr, extracted);

            fields[target_field] = mapped_value;
            set_alias_field(preview_event, target_field, mapped_value);
          }

          const json constants = rule.value("constants", json::object());
          for (auto it = constants.begin(); it != constants.end(); ++it) {
            preview_event[it.key()] = it.value();
          }

          preview_event["fields"] = fields;
          preview_event["extracted"] = extracted;
          preview_event["parser_status"] = "parsed";
          preview_event["parser_rule_id"] = safe_string(rule, "id", "");
          preview_event["parser_rule_name"] = safe_string(rule, "name", "");
          preview_event["policy_group_id"] = safe_string(rule, "policy_group_id", "");
        }

        send_json_response(res, 200, {
          {"status", "ok"},
          {"matched", matched},
          {"extracted", extracted},
          {"event_preview", preview_event},
          {"error", err}
        });
      } catch (const std::exception& e) {
        send_json_response(res, 400, {{"status", "error"}, {"message", e.what()}});
      }
    });

    // Rules API
    srv.Get("/api/rules", [&](const httplib::Request& req, httplib::Response& res) {
      AuthSession session;
      if (!require_role(req, res, "analyst", session)) return;

      std::lock_guard<std::mutex> lock(rules_file_mutex);

      json rules;
      std::string err;

      if (!read_json_file(rules_path, rules, err)) {
        send_json_response(res, 500, {
          {"status", "error"},
          {"message", err}
        });
        return;
      }

      if (!rules.is_array()) {
        send_json_response(res, 500, {
          {"status", "error"},
          {"message", "rules file must contain JSON array"}
        });
        return;
      }

      send_json_response(res, 200, {
        {"status", "ok"},
        {"count", rules.size()},
        {"rules", rules}
      });
    });

    srv.Post("/api/rules", [&](const httplib::Request& req, httplib::Response& res) {
      AuthSession session;
      if (!require_role(req, res, "admin", session)) return;

      std::lock_guard<std::mutex> lock(rules_file_mutex);

      try {
        json new_rule = json::parse(req.body);

        std::string err;
        if (!validate_rule_json(new_rule, err)) {
          send_json_response(res, 400, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        json rules;
        if (!read_json_file(rules_path, rules, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        if (!rules.is_array()) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", "rules file must contain JSON array"}
          });
          return;
        }

        const std::string id = new_rule["id"].get<std::string>();

        if (find_rule_index_by_id(rules, id) >= 0) {
          send_json_response(res, 409, {
            {"status", "error"},
            {"message", "rule already exists"},
            {"id", id}
          });
          return;
        }

        rules.push_back(new_rule);

        if (!write_json_file(rules_path, rules, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        const bool reloaded = detector.reload_rules();

        send_json_response(res, 201, {
          {"status", "ok"},
          {"action", "created"},
          {"id", id},
          {"rules_reloaded", reloaded},
          {"rule", new_rule}
        });
      } catch (const std::exception& e) {
        send_json_response(res, 400, {
          {"status", "error"},
          {"message", e.what()}
        });
      }
    });

    srv.Put(R"(/api/rules/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
      AuthSession session;
      if (!require_role(req, res, "admin", session)) return;

      std::lock_guard<std::mutex> lock(rules_file_mutex);

      try {
        const std::string id = req.matches[1].str();

        json updated_rule = json::parse(req.body);
        updated_rule["id"] = id;

        std::string err;
        if (!validate_rule_json(updated_rule, err)) {
          send_json_response(res, 400, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        json rules;
        if (!read_json_file(rules_path, rules, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        if (!rules.is_array()) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", "rules file must contain JSON array"}
          });
          return;
        }

        const int index = find_rule_index_by_id(rules, id);
        if (index < 0) {
          send_json_response(res, 404, {
            {"status", "error"},
            {"message", "rule not found"},
            {"id", id}
          });
          return;
        }

        rules[static_cast<std::size_t>(index)] = updated_rule;

        if (!write_json_file(rules_path, rules, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        const bool reloaded = detector.reload_rules();

        send_json_response(res, 200, {
          {"status", "ok"},
          {"action", "updated"},
          {"id", id},
          {"rules_reloaded", reloaded},
          {"rule", updated_rule}
        });
      } catch (const std::exception& e) {
        send_json_response(res, 400, {
          {"status", "error"},
          {"message", e.what()}
        });
      }
    });

    srv.Delete(R"(/api/rules/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
      AuthSession session;
      if (!require_role(req, res, "admin", session)) return;

      std::lock_guard<std::mutex> lock(rules_file_mutex);

      try {
        const std::string id = req.matches[1].str();

        json rules;
        std::string err;

        if (!read_json_file(rules_path, rules, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        if (!rules.is_array()) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", "rules file must contain JSON array"}
          });
          return;
        }

        const int index = find_rule_index_by_id(rules, id);
        if (index < 0) {
          send_json_response(res, 404, {
            {"status", "error"},
            {"message", "rule not found"},
            {"id", id}
          });
          return;
        }

        json removed_rule = rules[static_cast<std::size_t>(index)];
        rules.erase(rules.begin() + index);

        if (!write_json_file(rules_path, rules, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        const bool reloaded = detector.reload_rules();

        send_json_response(res, 200, {
          {"status", "ok"},
          {"action", "deleted"},
          {"id", id},
          {"rules_reloaded", reloaded},
          {"rule", removed_rule}
        });
      } catch (const std::exception& e) {
        send_json_response(res, 400, {
          {"status", "error"},
          {"message", e.what()}
        });
      }
    });

    const std::string dashboards_path = "config/dashboards.json";
    std::mutex dashboards_file_mutex;

    // Dashboards API
    srv.Get("/api/dashboards", [&](const httplib::Request& req, httplib::Response& res) {
      AuthSession session;
      if (!require_login(req, res, session)) return;

      std::lock_guard<std::mutex> lock(dashboards_file_mutex);

      json root;
      std::string err;

      if (!ensure_dashboards_file(dashboards_path, err)) {
        send_json_response(res, 500, {
          {"status", "error"},
          {"message", err}
        });
        return;
      }

      if (!read_json_file(dashboards_path, root, err)) {
        send_json_response(res, 500, {
          {"status", "error"},
          {"message", err}
        });
        return;
      }

      if (!normalize_dashboards_root(root, err)) {
        send_json_response(res, 500, {
          {"status", "error"},
          {"message", err}
        });
        return;
      }

      send_json_response(res, 200, {
        {"status", "ok"},
        {"version", root.value("version", 1)},
        {"count", root["dashboards"].size()},
        {"dashboards", root["dashboards"]}
      });
    });

    srv.Post("/api/dashboards", [&](const httplib::Request& req, httplib::Response& res) {
      AuthSession session;
      if (!require_role(req, res, "admin", session)) return;

      std::lock_guard<std::mutex> lock(dashboards_file_mutex);

      try {
        json dashboard = json::parse(req.body);

        if (!dashboard.is_object()) {
          send_json_response(res, 400, {
            {"status", "error"},
            {"message", "dashboard must be JSON object"}
          });
          return;
        }

        if (!dashboard.contains("id") || !dashboard["id"].is_string() || dashboard["id"].get<std::string>().empty()) {
          dashboard["id"] = make_dashboard_id(safe_string(dashboard, "title", "Custom dashboard"));
        }

        const std::string now = now_iso_utc();

        if (!dashboard.contains("created_at") || !dashboard["created_at"].is_string()) {
          dashboard["created_at"] = now;
        }

        dashboard["updated_at"] = now;

        if (!dashboard.contains("locked")) {
          dashboard["locked"] = false;
        }

        if (!dashboard.contains("layout")) {
          dashboard["layout"] = {
            {"columns", 2}
          };
        }

        std::string err;
        if (!validate_dashboard_json(dashboard, err)) {
          send_json_response(res, 400, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        if (!ensure_dashboards_file(dashboards_path, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        json root;
        if (!read_json_file(dashboards_path, root, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        if (!normalize_dashboards_root(root, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        const std::string id = dashboard["id"].get<std::string>();

        if (find_dashboard_index_by_id(root, id) >= 0) {
          send_json_response(res, 409, {
            {"status", "error"},
            {"message", "dashboard already exists"},
            {"id", id}
          });
          return;
        }

        root["dashboards"].push_back(dashboard);

        if (!write_json_file(dashboards_path, root, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        send_json_response(res, 201, {
          {"status", "ok"},
          {"action", "created"},
          {"id", id},
          {"dashboard", dashboard}
        });
      } catch (const std::exception& e) {
        send_json_response(res, 400, {
          {"status", "error"},
          {"message", e.what()}
        });
      }
    });

    srv.Put(R"(/api/dashboards/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
      AuthSession session;
      if (!require_role(req, res, "admin", session)) return;

      std::lock_guard<std::mutex> lock(dashboards_file_mutex);

      try {
        const std::string id = req.matches[1].str();

        json updated_dashboard = json::parse(req.body);
        updated_dashboard["id"] = id;
        updated_dashboard["updated_at"] = now_iso_utc();

        std::string err;

        if (!ensure_dashboards_file(dashboards_path, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        json root;
        if (!read_json_file(dashboards_path, root, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        if (!normalize_dashboards_root(root, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        const int index = find_dashboard_index_by_id(root, id);

        if (index < 0) {
          send_json_response(res, 404, {
            {"status", "error"},
            {"message", "dashboard not found"},
            {"id", id}
          });
          return;
        }

        json old_dashboard = root["dashboards"][static_cast<std::size_t>(index)];

        if (old_dashboard.contains("created_at") && old_dashboard["created_at"].is_string()) {
          updated_dashboard["created_at"] = old_dashboard["created_at"];
        }

        if (old_dashboard.contains("locked") && old_dashboard["locked"].is_boolean()) {
          updated_dashboard["locked"] = old_dashboard["locked"];
        } else if (!updated_dashboard.contains("locked")) {
          updated_dashboard["locked"] = false;
        }

        if (!updated_dashboard.contains("layout")) {
          updated_dashboard["layout"] = {
            {"columns", 2}
          };
        }

        if (!validate_dashboard_json(updated_dashboard, err)) {
          send_json_response(res, 400, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        root["dashboards"][static_cast<std::size_t>(index)] = updated_dashboard;

        if (!write_json_file(dashboards_path, root, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        send_json_response(res, 200, {
          {"status", "ok"},
          {"action", "updated"},
          {"id", id},
          {"dashboard", updated_dashboard}
        });
      } catch (const std::exception& e) {
        send_json_response(res, 400, {
          {"status", "error"},
          {"message", e.what()}
        });
      }
    });

    srv.Delete(R"(/api/dashboards/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
      AuthSession session;
      if (!require_role(req, res, "admin", session)) return;

      std::lock_guard<std::mutex> lock(dashboards_file_mutex);

      try {
        const std::string id = req.matches[1].str();

        json root;
        std::string err;

        if (!ensure_dashboards_file(dashboards_path, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        if (!read_json_file(dashboards_path, root, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        if (!normalize_dashboards_root(root, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        const int index = find_dashboard_index_by_id(root, id);

        if (index < 0) {
          send_json_response(res, 404, {
            {"status", "error"},
            {"message", "dashboard not found"},
            {"id", id}
          });
          return;
        }

        json removed_dashboard = root["dashboards"][static_cast<std::size_t>(index)];

        if (removed_dashboard.value("locked", false)) {
          send_json_response(res, 409, {
            {"status", "error"},
            {"message", "locked dashboard cannot be deleted"},
            {"id", id}
          });
          return;
        }

        root["dashboards"].erase(root["dashboards"].begin() + index);

        if (!write_json_file(dashboards_path, root, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        send_json_response(res, 200, {
          {"status", "ok"},
          {"action", "deleted"},
          {"id", id},
          {"dashboard", removed_dashboard}
        });
      } catch (const std::exception& e) {
        send_json_response(res, 400, {
          {"status", "error"},
          {"message", e.what()}
        });
      }
    });

    // Ingest endpoint
    srv.Post("/ingest", [&](const httplib::Request& req, httplib::Response& res) {
      if (!require_agent_token(req, res)) return;

      try {
        auto j = json::parse(req.body);

        if (!j.is_object()) {
          throw std::runtime_error("ingest body must be a JSON object");
        }

        const std::string received_at = now_iso_utc();

        // Заполняем минимальные поля, если агент не прислал
        std::string ts = safe_string(j, "ts", received_at);
        std::string event_type = safe_string(j, "event_type", "unknown");
        std::string source = safe_string(j, "source", "unknown");
        std::string event_id = safe_string(j, "event_id", "");

	if (!j.contains("ts")) j["ts"] = ts;
	if (!j.contains("event_type")) j["event_type"] = event_type;
	if (!j.contains("source")) j["source"] = source;

        if (event_id.empty()) {
          event_id = generate_event_id();
        }

        j["event_id"] = event_id;
        j["received_at"] = received_at;

        std::string source_type = safe_string(j, "source_type", "");
        if (source_type.empty()) {
          source_type = detect_source_type(j);
        }
        j["source_type"] = source_type;

        if (j.contains("receiver_id") && j["receiver_id"].is_string()) {
          apply_parser_rules(j, parser_rules_path);
        } else if (!j.contains("parser_status")) {
          j["parser_status"] = "built_in";
        }

        ts = safe_string(j, "ts", received_at);
        event_type = safe_string(j, "event_type", "unknown");
        source = safe_string(j, "source", "unknown");

        // Канонизируем json (храним строкой)
        std::string body = j.dump();

        db.insert_event(event_id, ts, received_at, event_type, source, source_type, body);

	broadcast_sse("event", make_event_response_item(0, j));

	try {
          const std::string parser_status = safe_string(j, "parser_status", "built_in");

          if (parser_status == "unparsed") {
            json out = {
              {"status", "ok"},
              {"event_id", event_id},
              {"received_at", received_at},
              {"source_type", source_type},
              {"parser_status", parser_status}
            };

            res.set_content(out.dump(), "application/json");
            res.status = 200;
            return;
          }

	  auto detected_alerts = detector.process_event(j);
	  for (const auto& alert : detected_alerts){
	    db.insert_alert(
	      safe_string(alert, "ts", now_iso_utc()),
              safe_string(alert, "rule_name", "unknown_rule"),
              safe_string(alert, "severity", "info"),
              safe_string(alert, "title", "Alert"),
              safe_string(alert, "description", ""),
              alert.dump()
            );
	    broadcast_sse("alert", make_alert_response_item(alert));
	  }

      	  auto correlated_alerts = correlator.process_event(j);
	  for (const auto& alert : correlated_alerts){
	    db.insert_alert(
	      safe_string(alert, "ts", now_iso_utc()),
              safe_string(alert, "rule_name", "unknown_rule"),
              safe_string(alert, "severity", "info"),
              safe_string(alert, "title", "Alert"),
              safe_string(alert, "description", ""),
              alert.dump()
            );
	    broadcast_sse("alert", make_alert_response_item(alert));
	  }
	} catch (const std::exception& e) {
	  std::cerr << "[detect][ERR] " << e.what() << "\n";
	} catch (...){
	  std::cerr << "[detect][ERR] unknown detection error\n";
	}

        json out = {
          {"status", "ok"},
          {"event_id", event_id},
          {"received_at", received_at},
          {"source_type", source_type}
        };

        res.set_content(out.dump(), "application/json");
        res.status = 200;
      } catch (const std::exception& e) {
	std::cerr << "[ingest][ERR] " << e.what() << "\n";
        json out = {{"status", "error"}, {"message", e.what()}};
        res.set_content(out.dump(), "application/json");
        res.status = 400;
      }
    });

    // Dashboard stats API
    srv.Get("/api/stats", [&](const httplib::Request& req, httplib::Response& res) {
      AuthSession session;
      if (!require_login(req, res, session)) return;

      try {
        const auto events_by_type = db.count_events_by_event_type();
        const auto events_by_source_type = db.count_events_by_source_type();
        const auto alerts_by_severity = db.count_alerts_by_severity();
        const auto events_over_time = db.count_events_over_time(60);

        json events_by_type_json = json::object();
        for (const auto& kv : events_by_type) {
          events_by_type_json[kv.first] = kv.second;
        }

        json events_by_source_type_json = json::object();
        for (const auto& kv : events_by_source_type) {
          events_by_source_type_json[kv.first] = kv.second;
        }

        json alerts_by_severity_json = json::object();
        for (const auto& kv : alerts_by_severity) {
          alerts_by_severity_json[kv.first] = kv.second;
        }

        json events_over_time_json = json::array();
        for (const auto& row : events_over_time) {
          events_over_time_json.push_back({
            {"bucket", row.bucket},
            {"count", row.count}
          });
        }

        long long high_alerts = 0;
        long long critical_alerts = 0;

        auto high_it = alerts_by_severity.find("high");
        if (high_it != alerts_by_severity.end()) {
          high_alerts = high_it->second;
        }

        auto critical_it = alerts_by_severity.find("critical");
        if (critical_it != alerts_by_severity.end()) {
          critical_alerts = critical_it->second;
        }

        json out = {
          {"status", "ok"},
          {"total_events", db.count_all_events()},
          {"total_alerts", db.count_all_alerts()},
          {"high_alerts", high_alerts},
          {"critical_alerts", critical_alerts},
          {"events_by_type", events_by_type_json},
          {"events_by_source_type", events_by_source_type_json},
          {"alerts_by_severity", alerts_by_severity_json},
          {"events_over_time", events_over_time_json}
        };

        send_json_response(res, 200, out);
      } catch (const std::exception& e) {
        send_json_response(res, 500, {
          {"status", "error"},
          {"message", e.what()}
        });
      }
    });

    // Get last events
    srv.Get("/api/events", [&](const httplib::Request& req, httplib::Response& res) {
      AuthSession session;
      if (!require_login(req, res, session)) return;

      int limit = config.dashboard.events_limit_default;
      if (req.has_param("limit")) {
        try {
          limit = std::stoi(req.get_param_value("limit"));
        } catch (...) {
          limit = config.dashboard.events_limit_default;
        }
      }
      limit = clamp_limit(limit, config.dashboard.events_limit_default, config.dashboard.events_limit_max);

      auto rows = db.get_last_events(limit);

      json arr = json::array();
      for (const auto& r : rows) {
        // json внутри таблицы — это строка, попробуем распарсить назад
        json original;
        try {
          original = json::parse(r.json);
        } catch (...) {
          original = json::object({{"raw_json", r.json}});
        }

	json item = {
 	  {"id", r.id},
 	  {"event_id", r.event_id},
 	  {"ts", r.ts},
 	  {"received_at", r.received_at},
 	  {"host", r.host},
 	  {"event_type", r.event_type},
 	  {"source", r.source},
 	  {"source_type", r.source_type},
 	  {"severity", r.severity},

 	  {"receiver_id", safe_string(original, "receiver_id", "")},
 	  {"receiver_name", safe_string(original, "receiver_name", "")},
 	  {"parser_status", safe_string(original, "parser_status", "")},
 	  {"parser_rule_id", safe_string(original, "parser_rule_id", "")},
 	  {"parser_rule_name", safe_string(original, "parser_rule_name", "")},
 	  {"policy_group_id", safe_string(original, "policy_group_id", "")},
 	  {"event_name", safe_string(original, "event_name", "")},

 	  {"event", original}
	};

        arr.push_back(std::move(item));
      }

      json out = {{"events", arr}};
      res.set_content(out.dump(), "application/json");
      res.status = 200;
    });

    // Get last alerts
    srv.Get("/api/alerts", [&](const httplib::Request& req, httplib::Response& res) {
      AuthSession session;
      if (!require_login(req, res, session)) return;

      int limit = config.dashboard.alerts_limit_default;
      if (req.has_param("limit")) {
        try {
          limit = std::stoi(req.get_param_value("limit"));
        } catch (...) {
          limit = config.dashboard.alerts_limit_default;
        }
      }
      limit = clamp_limit(limit, config.dashboard.alerts_limit_default, config.dashboard.alerts_limit_max);

      auto rows = db.get_last_alerts(limit);

      json arr = json::array();
      for (const auto& r : rows) {
        json original;
        try {
          original = json::parse(r.json);
        } catch (...) {
          original = json::object({{"raw_json", r.json}});
        }

        json item = {
          {"id", r.id},
          {"ts", r.ts},
          {"rule_name", r.rule_name},
          {"severity", r.severity},
          {"title", r.title},
          {"description", r.description},
          {"alert", original}
        };
        arr.push_back(std::move(item));
      }

      json out = {{"alerts", arr}};
      res.set_content(out.dump(), "application/json");
      res.status = 200;
    });

    std::cout << "[mini-siem-server] listening on http://"
              << config.server.host << ":" << config.server.port << "\n";
    srv.listen(config.server.host.c_str(), config.server.port);
  } catch (const std::exception& e) {
    std::cerr << "Fatal: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
