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
std::string safe_string(const json& j, const char* key, const std::string& def = "") {
  if (!j.contains(key) || j[key].is_null()) return def;
  if (j[key].is_string()) return j[key].get<std::string>();
  return j[key].dump();
}

} //NAMESPACE END


DetectionEngine::DetectionEngine(SqliteDb& db) : db_(db) {}

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

  return alerts;
}

json DetectionEngine::detect_failed_login_by_ip(const json& event) {
  const std::string src_ip = safe_string(event, "src_ip", "");
  if (src_ip.empty()) return nullptr;

  const std::string since_ts = minutes_ago_iso_utc(5);
  const long long count = db_.count_auth_failed_by_src_ip_since(src_ip, since_ts);

  std::cerr << "[detect] ip rule src_ip=" << src_ip
          << " count=" << count
          << " since=" << since_ts << "\n";
  if (count < 5) return nullptr;

  const std::string now = now_iso_utc();
  const std::string rule_name = "failed_login_threshold_by_ip";
  const std::string severity = "high";
  const std::string title = "Multiple failed logins from one IP";

  std::ostringstream desc;
  desc << "Detected " << count << " failed login attempts from IP " << src_ip 
       << " within the last 5 minutes.";

  return json {
    {"ts", now},
    {"rule_name", rule_name},
    {"severity", severity},
    {"title", title},
    {"description", desc.str()},
    {"src_ip", src_ip},
    {"count", count},
    {"window_minutes", 5}
  };

  //db_.insert_alert(now, rule_name, severity, title, desc.str(), alert.dump());
}

json DetectionEngine::detect_failed_login_by_user(const json& event) {
  const std::string user = safe_string(event, "user", "");
  if (user.empty()) return nullptr;

  const std::string since_ts = minutes_ago_iso_utc(5);
  const long long count = db_.count_auth_failed_by_user_since(user, since_ts);

  std::cerr << "[detect] user rule user=" << user
          << " count=" << count
          << " since=" << since_ts << "\n";
  if (count < 5) return nullptr;

  const std::string now = now_iso_utc();
  const std::string rule_name = "failed_login_threshold_by_user";
  const std::string severity = "high";
  const std::string title = "Multiple failed logins from one user";

  std::ostringstream desc;
  desc << "Detected " << count << " failed login attempts from user " << user
       << " within the last 5 minutes.";

  return json {
    {"ts", now},
    {"rule_name", rule_name},
    {"severity", severity},
    {"title", title},
    {"description", desc.str()},
    {"user", user},
    {"count", count},
    {"window_minutes", 5}
  };

 // db_.insert_alert(now, rule_name, severity, title, desc.str(), alert.dump());
}

json DetectionEngine::detect_invalid_user_by_ip(const json& event) {
  const std::string src_ip = safe_string(event, "src_ip", "");
  if (src_ip.empty()) return nullptr;

  const std::string since_ts = minutes_ago_iso_utc(5);
  const long long count = db_.count_auth_invalid_user_by_src_ip_since(src_ip, since_ts);

  std::cerr << "[detect] invalid-user rule src_ip=" << src_ip
            << " count=" << count
            << " since=" << since_ts << "\n";
  if (count < 5) return nullptr;

  const std::string now = now_iso_utc();
  const std::string rule_name = "invalid_user_threshold_by_ip";
  const std::string severity = "medium";
  const std::string title = "Multiple invalid-user attempts from one IP";

  std::ostringstream desc;
  desc << "Detected " << count << " invalid-user login attempts from IP " << src_ip
       << " within the last 5 minutes.";

  return json {
    {"ts", now},
    {"rule_name", rule_name},
    {"severity", severity},
    {"title", title},
    {"description", desc.str()},
    {"src_ip", src_ip},
    {"count", count},
    {"window_minutes", 5}
  };

  //db_.insert_alert(now, rule_name, severity, title, desc.str(), alert.dump());
}

json DetectionEngine::detect_suspicious_sudo_by_user(const json& event) {
  const std::string user = safe_string(event, "user", "");
  if (user.empty()) return nullptr;

  const std::string since_ts = minutes_ago_iso_utc(5);
  const long long count = db_.count_privilege_escalation_by_user_since(user, since_ts);

  std::cerr << "[detect] sudo rule user=" << user
            << " count=" << count
            << " since=" << since_ts << "\n";
  if (count < 3) return nullptr;

  const std::string now = now_iso_utc();
  const std::string rule_name = "suspicious_sudo_usage_by_user";
  const std::string severity = "medium";
  const std::string title = "Suspiciously frequent sudo usage";

  std::ostringstream desc;
  desc << "Detected " << count << " privilege escalation events for user " << user
       << " within the last 5 minutes.";

  return json {
    {"ts", now},
    {"rule_name", rule_name},
    {"severity", severity},
    {"title", title},
    {"description", desc.str()},
    {"user", user},
    {"count", count},
    {"window_minutes", 5}
  };

  //db_.insert_alert(now, rule_name, severity, title, desc.str(), alert.dump());
}

json DetectionEngine::detect_too_frequent_logins_by_user(const json& event) {
  const std::string user = safe_string(event, "user", "");
  if (user.empty()) return nullptr;

  const std::string since_ts = minutes_ago_iso_utc(2);
  const long long count = db_.count_auth_success_by_user_since(user, since_ts);

  std::cerr << "[detect] success-frequency rule user=" << user
            << " count=" << count
            << " since=" << since_ts << "\n";
  if (count < 10) return nullptr;

  const std::string now = now_iso_utc();
  const std::string rule_name = "too_frequent_logins_by_user";
  const std::string severity = "medium";
  const std::string title = "Too many successful logins in a short period";

  std::ostringstream desc;
  desc << "Detected " << count << " successful logins for user " << user
       << " within the last 2 minutes.";

  return json {
    {"ts", now},
    {"rule_name", rule_name},
    {"severity", severity},
    {"title", title},
    {"description", desc.str()},
    {"user", user},
    {"count", count},
    {"window_minutes", 2}
  };

  //db_.insert_alert(now, rule_name, severity, title, desc.str(), alert.dump());
}
