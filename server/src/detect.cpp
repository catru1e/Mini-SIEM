#include "detect.hpp"

#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream> //std::ostringstream
#include <vector>

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
std::string minutes_ago_iso_utc(int minutes) {
  using namespace std::chrono;
  auto now = system_clock::now() - std::chrono::minutes(minutes);
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

int seconds_to_minutes_rounded_up(int seconds){
  if (seconds <= 0) return 0;
  return (seconds +59) / 60;
}

std::string join_commands(const std::vector<std::string>& commands) {
  if (commands.empty()) return "";

  std::ostringstream ss;
  for (size_t i = 0; i < commands.size(); ++i) {
    if (i > 0) ss << "; ";
    ss << commands[i];
  }
  return ss.str();
}
constexpr int ALERT_SUPPRESSION_SECONDS = 60;
} //NAMESPACE END


DetectionEngine::DetectionEngine(SqliteDb& db, DetectionConfig config)
   : db_(db), config_(std::move(config)) {}

std::vector<json> DetectionEngine::process_event(const json& event) {
  std::vector<json> alerts;
  const std::string event_type = safe_string(event, "event_type", "");

  std::cerr << "[detect] process_event type=" << event_type << "\n";

  if (event_type == "auth_failed") {
    auto a1 = detect_failed_login_by_ip(event);
    if (!a1.is_null()) alerts.push_back(std::move(a1));

    auto a2 = detect_failed_login_by_user(event);
    if (!a2.is_null()) alerts.push_back(std::move(a2));
    return alerts;
  }
  if (event_type == "auth_invalid_user") {
    auto a = detect_invalid_user_by_ip(event);
    if (!a.is_null()) alerts.push_back(std::move(a));
    return alerts;
  }

  if (event_type == "privilege_escalation") {
    auto a = detect_suspicious_sudo_by_user(event);
    if(!a.is_null()) alerts.push_back(std::move(a));
    return alerts;
  }

  if (event_type == "auth_success") {
    auto a = detect_too_frequent_logins_by_user(event);
    if (!a.is_null()) alerts.push_back(std::move(a));
    return alerts;
  }

  if (event_type == "process_start") {
    auto a = detect_blacklisted_process(event);
    if (!a.is_null()) alerts.push_back(std::move(a));
    return alerts;
  }

  if (event_type == "file_created" || event_type == "file_modified" || event_type == "file_deleted") {
    auto a = detect_sensitive_ssh_file_change(event);
    if (!a.is_null()) alerts.push_back(std::move(a));
    return alerts;
  }

  return alerts;
}

json DetectionEngine::detect_failed_login_by_ip(const json& event) {
  const std::string src_ip = safe_string(event, "src_ip", "");
  if (src_ip.empty()) return nullptr;

  const int window_seconds = config_.failed_login_window_seconds;
  const int threshold = config_.failed_login_threshold;
  const std::string since_ts = minutes_ago_iso_utc(window_seconds);
  const long long count = db_.count_auth_failed_by_src_ip_since(src_ip, since_ts);

  std::cerr << "[detect] ip rule src_ip=" << src_ip
          << " count=" << count
          << " since=" << since_ts << "\n";
  if (count < threshold) return nullptr;

  const std::string now = now_iso_utc();
  const std::string rule_name = "failed_login_threshold_by_ip";
  const std::string severity = "high";
  const std::string title = "Multiple failed logins from one IP";

  std::ostringstream desc;
  desc << "Detected " << count << " failed login attempts from IP " << src_ip
       << " within the last " << window_seconds << " seconds.";
  return json {
    {"ts", now},
    {"rule_name", rule_name},
    {"severity", severity},
    {"title", title},
    {"description", desc.str()},
    {"src_ip", src_ip},
    {"count", count},
    {"threshold", threshold},
    {"window_seconds", window_seconds},
    {"window_minutes", seconds_to_minutes_rounded_up(window_seconds)}
  };
}

json DetectionEngine::detect_failed_login_by_user(const json& event) {
  const std::string user = safe_string(event, "user", "");
  if (user.empty()) return nullptr;

  const int window_seconds = config_.failed_login_window_seconds;
  const int threshold = config_.failed_login_threshold;
  const std::string since_ts = minutes_ago_iso_utc(window_seconds);
  const long long count = db_.count_auth_failed_by_user_since(user, since_ts);

  std::cerr << "[detect] user rule user=" << user
          << " count=" << count
          << " since=" << since_ts << "\n";
  if (count < threshold) return nullptr;

  const std::string now = now_iso_utc();
  const std::string rule_name = "failed_login_threshold_by_user";
  const std::string severity = "high";
  const std::string title = "Multiple failed logins from one user";

  std::ostringstream desc;
  desc << "Detected " << count << " failed login attempts from user " << user
       << " within the last " << window_seconds << " seconds.";

  return json {
    {"ts", now},
    {"rule_name", rule_name},
    {"severity", severity},
    {"title", title},
    {"description", desc.str()},
    {"user", user},
    {"count", count},
    {"threshold", threshold},
    {"window_seconds", window_seconds},
    {"window_minutes", seconds_to_minutes_rounded_up(window_seconds)}
  };
}

json DetectionEngine::detect_invalid_user_by_ip(const json& event) {
  const std::string src_ip = safe_string(event, "src_ip", "");
  if (src_ip.empty()) return nullptr;

  const int window_seconds = config_.invalid_user_window_seconds;
  const int threshold = config_.invalid_user_threshold;
  const std::string since_ts = minutes_ago_iso_utc(window_seconds);
  const long long count = db_.count_auth_invalid_user_by_src_ip_since(src_ip, since_ts);

  std::cerr << "[detect] invalid-user rule src_ip=" << src_ip
            << " count=" << count
            << " since=" << since_ts << "\n";
  if (count < threshold) return nullptr;

  const std::string now = now_iso_utc();
  const std::string rule_name = "invalid_user_threshold_by_ip";
  const std::string severity = "medium";
  const std::string title = "Multiple invalid-user attempts from one IP";

  std::ostringstream desc;
  desc << "Detected " << count << " invalid-user login attempts from IP " << src_ip
       << " within the last " << window_seconds << " seconds.";

  return json {
    {"ts", now},
    {"rule_name", rule_name},
    {"severity", severity},
    {"title", title},
    {"description", desc.str()},
    {"src_ip", src_ip},
    {"count", count},
    {"threshold", threshold},
    {"window_seconds", window_seconds},
    {"window_minutes", seconds_to_minutes_rounded_up(window_seconds)}
  };
}

json DetectionEngine::detect_suspicious_sudo_by_user(const json& event) {
  const std::string user = safe_string(event, "user", "");
  if (user.empty()) return nullptr;

  const int window_seconds = config_.sudo_window_seconds;
  const int threshold = config_.sudo_threshold;
  const std::string since_ts = seconds_ago_iso_utc(window_seconds);
  const long long count = db_.count_privilege_escalation_by_user_since(user, since_ts);

  std::cerr << "[detect] sudo rule user=" << user
            << " count=" << count
            << " since=" << since_ts << "\n";
  if (count < threshold) return nullptr;

  const std::string rule_name = "suspicious_sudo_usage_by_user";
  const std::string suppression_since_ts = seconds_ago_iso_utc(ALERT_SUPPRESSION_SECONDS);

  if (db_.has_recent_alert_for_rule_and_user_since(rule_name, user, suppression_since_ts)) {
    std::cerr << "[detect] sudo rule suppressed for user=" << user
              << " within last " << ALERT_SUPPRESSION_SECONDS << " seconds\n";
    return nullptr;
  }

  const auto recent_commands =
      db_.get_recent_privilege_escalation_commands_by_user_since(user, since_ts, 5);

  const std::string now = now_iso_utc();
  const std::string severity = "medium";
  const std::string title = "Suspiciously frequent sudo usage";

  std::ostringstream desc;
  desc << "Detected " << count << " privilege escalation events for user " << user
       << " within the last " << window_seconds << " seconds.";

  if (!recent_commands.empty()) {
    desc << " Recent commands: " << join_commands(recent_commands);
  }

  return json {
    {"ts", now},
    {"rule_name", rule_name},
    {"severity", severity},
    {"title", title},
    {"description", desc.str()},
    {"user", user},
    {"count", count},
    {"threshold", threshold},
    {"window_seconds", window_seconds},
    {"window_minutes", seconds_to_minutes_rounded_up(window_seconds)},
    {"recent_commands", recent_commands}
  };
}

json DetectionEngine::detect_too_frequent_logins_by_user(const json& event) {
  const std::string user = safe_string(event, "user", "");
  if (user.empty()) return nullptr;

  const int window_seconds = config_.success_login_window_seconds;
  const int threshold = config_.success_login_threshold;
  const std::string since_ts = seconds_ago_iso_utc(window_seconds);
  const long long count = db_.count_auth_success_by_user_since(user, since_ts);

  std::cerr << "[detect] success-frequency rule user=" << user
            << " count=" << count
            << " since=" << since_ts << "\n";
  if (count < threshold) return nullptr;

  const std::string now = now_iso_utc();
  const std::string rule_name = "too_frequent_logins_by_user";
  const std::string severity = "medium";
  const std::string title = "Too many successful logins in a short period";

  std::ostringstream desc;
  desc << "Detected " << count << " successful logins for user " << user
       << " within the last " << window_seconds << " seconds.";

  return json {
    {"ts", now},
    {"rule_name", rule_name},
    {"severity", severity},
    {"title", title},
    {"description", desc.str()},
    {"user", user},
    {"count", count},
    {"threshold", threshold},
    {"window_seconds", window_seconds},
    {"window_minutes", seconds_to_minutes_rounded_up(window_seconds)}
  };
}

json DetectionEngine::detect_blacklisted_process(const json& event) {
  const std::string process_name = safe_string(event, "process_name", "");
  const std::string cmdline = safe_string(event, "cmdline", "");

  if (process_name.empty()) return nullptr;

  bool matched = false;
  for (const auto& bad : config_.blacklisted_processes) {
    if (process_name == bad) {
      matched = true;
      break;
    }
  }

  if (!matched) return nullptr;

  const std::string now = now_iso_utc();
  const std::string rule_name = "blacklisted_process_detected";
  const std::string severity = "high";
  const std::string title = "Blacklisted process started";

  std::ostringstream desc;
  desc << "Detected blacklisted process: " << process_name;
  if (!cmdline.empty()) {
    desc << " cmdline=\"" << cmdline << "\"";
  }

  return json {
    {"ts", now},
    {"rule_name", rule_name},
    {"severity", severity},
    {"title", title},
    {"description", desc.str()},
    {"process_name", process_name},
    {"cmdline", cmdline},
    {"pid", event.contains("pid") ? event["pid"] : json(nullptr)}
  };
}


json DetectionEngine::detect_sensitive_ssh_file_change(const json& event) {
  const std::string path = safe_string(event, "path", "");
  const std::string event_type = safe_string(event, "event_type", "");
  if (path.empty()) return nullptr;

  bool sensitive = false;
  if (path.find("authorized_keys") != std::string::npos) sensitive = true;
  if (path.find("known_hosts") != std::string::npos) sensitive = true;
  if (path.find("config") != std::string::npos) sensitive = true;
  if (path.find("id_rsa") != std::string::npos) sensitive = true;
  if (path.find("id_ed25519") != std::string::npos) sensitive = true;

  if (!sensitive) return nullptr;

  const std::string now = now_iso_utc();
  const std::string rule_name = "sensitive_ssh_file_change";
  const std::string severity = "high";
  const std::string title = "Sensitive SSH file changed";

  std::ostringstream desc;
  desc << "Detected " << event_type << " on sensitive SSH file: " << path;

  return json {
    {"ts", now},
    {"rule_name", rule_name},
    {"severity", severity},
    {"title", title},
    {"description", desc.str()},
    {"path", path},
    {"event_type", event_type}
  };
}
