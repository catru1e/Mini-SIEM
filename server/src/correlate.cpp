#include "correlate.hpp"

#include <chrono>
#include <ctime>
#include <sstream>
#include <iostream>

using json = nlohmann::json;

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
		"%04d-%02d-%02dT%02d:%02d:%02d.000Z",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                tm.tm_hour, tm.tm_min, tm.tm_sec);
  return std::string(buf);
}

std::string safe_string(const json& j, const char* key, const std::string& def = "") {
  if (!j.contains(key) || j[key].is_null()) return def;
  if (j[key].is_string()) return j[key].get<std::string>();
  return j[key].dump();
}

} //namespace :)

CorrelationEngine::CorrelationEngine(SqliteDb& db) : db_(db) {}

void CorrelationEngine::process_event(const json& event) {
  const std::string event_type = safe_string(event, "event_type");

  if (event_type == "auth_success") {
    correlate_bruteforce_success(event);
  }
}

void CorrelationEngine::correlate_bruteforce_success(const json& event) {
  const std::string src_ip = safe_string(event, "src_ip", "");
  if (src_ip.empty()) return;

  const std::string since_ts = minutes_ago_iso_utc(5);

  long long failed_count = db_.count_auth_failed_by_src_ip_since(src_ip, since_ts);

  std::cerr << "[corr] bruteforce check ip=" << src_ip
	    << " failed_count=" << failed_count
            << " since=" << since_ts << "\n";

  if (failed_count < 5) return;

  const std::string now = now_iso_utc();
  const std::string rule_name = "bruteforce_success_sequence";
  const std::string severity = "high";
  const std::string title = "Possible brute-force success";

  std::ostringstream desc;
  desc << "IP " << src_ip
       << " had " << failed_count
       << " failed logins followed by a successful login.";

  json alert = {
    {"ts", now},
    {"rule_name", rule_name},
    {"severity", severity},
    {"title", title},
    {"description", desc.str()},
    {"src_ip", src_ip},
    {"failed_count", failed_count},
    {"window_minutes", 5}
  };

  db_.insert_alert(now, rule_name, severity, title, desc.str(), alert.dump());
}
