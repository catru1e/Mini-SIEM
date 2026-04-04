#include "config.hpp"
#include "json.hpp"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>

using json = nlohmann::json;

namespace {
std::string read_text_file(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in.is_open()) {
    throw std::runtime_error("failed to open config file: " + path);
  }
  std::ostringstream ss;
  ss << in.rdbuf();
  return ss.str();
}

std::string expand_home_path(const std::string& path) {
  if (path == "~") {
    const char* home = std::getenv("HOME");
    return home ? std::string(home) : path;
  }
  if (path.rfind("~/", 0) == 0) {
    const char* home = std::getenv("HOME");
    if(!home) return path;
    return std::string(home) + path.substr(1);
  }
  return path;
}

int read_int_or_default(const json& j, const char* key, int def) {
  if (!j.contains(key) || j[key].is_null()) return def;
  if (!j[key].is_number_integer()) return def;
  return j[key].get<int>();
}

std::string read_string_or_default(const json& j, const char* key, const std::string& def) {
  if (!j.contains(key) || j[key].is_null()) return def;
  if (!j[key].is_string()) return def;
  return j[key].get<std::string>();
}

std::vector<std::string> read_string_array_or_default(const json& j,
						      const char* key,
						      const std::vector<std::string>& def){
  if (!j.contains(key) || !j[key].is_array()) return def;

  std::vector<std::string> out;
  for (const auto& item : j[key]){
    if (item.is_string()) {
      out.push_back(item.get<std::string>());
    }
  }

  if (out.empty()) return def;
  return out;
}

} // NAMESPACE END :)

AppConfig ConfigLoader::load_from_file(const std::string& path) {
  const std::string text = read_text_file(path);
  json root = json::parse(text);

  AppConfig cfg;

  if (root.contains("server") && root["server"].is_object()) {
    const auto& s = root["server"];
    cfg.server.host = read_string_or_default(s, "host", cfg.server.host);
    cfg.server.port = read_int_or_default(s, "port", cfg.server.port);
    cfg.server.db_path = read_string_or_default(s, "db_path", cfg.server.db_path);
  }

  if (root.contains("agent") && root["agent"].is_object()) {
    const auto& a = root["agent"];
    cfg.agent.log_paths = read_string_array_or_default(a, "log_paths", cfg.agent.log_paths);
    cfg.agent.state_path = read_string_or_default(a, "state_path", cfg.agent.state_path);
    cfg.agent.agent_log_path = read_string_or_default(a, "agent_log_path", cfg.agent.agent_log_path);
    cfg.agent.ssh_watch_path = read_string_or_default(a, "ssh_watch_path", cfg.agent.ssh_watch_path);
    cfg.agent.ssh_watch_path = expand_home_path(cfg.agent.ssh_watch_path);
  }

  if (root.contains("detection") && root["detection"].is_object()) {
    const auto& d = root["detection"];
    cfg.detection.failed_login_threshold =
	read_int_or_default(d, "failed_login_threshold", cfg.detection.failed_login_threshold);
    cfg.detection.failed_login_window_seconds =
        read_int_or_default(d, "failed_login_window_seconds", cfg.detection.failed_login_window_seconds);

    cfg.detection.invalid_user_threshold =
        read_int_or_default(d, "invalid_user_threshold", cfg.detection.invalid_user_threshold);
    cfg.detection.invalid_user_window_seconds =
        read_int_or_default(d, "invalid_user_window_seconds", cfg.detection.invalid_user_window_seconds);

    cfg.detection.sudo_threshold =
        read_int_or_default(d, "sudo_threshold", cfg.detection.sudo_threshold);
    cfg.detection.sudo_window_seconds =
        read_int_or_default(d, "sudo_window_seconds", cfg.detection.sudo_window_seconds);

    cfg.detection.success_login_threshold =
        read_int_or_default(d, "success_login_threshold", cfg.detection.success_login_threshold);
    cfg.detection.success_login_window_seconds =
        read_int_or_default(d, "success_login_window_seconds", cfg.detection.success_login_window_seconds);

    cfg.detection.blacklisted_processes =
        read_string_array_or_default(d, "blacklisted_processes", cfg.detection.blacklisted_processes);
  }

  if (root.contains("dashboard") && root["dashboard"].is_object()) {
    const auto& d = root["dashboard"];
    cfg.dashboard.events_limit_default =
        read_int_or_default(d, "events_limit_default", cfg.dashboard.events_limit_default);
    cfg.dashboard.alerts_limit_default =
        read_int_or_default(d, "alerts_limit_default", cfg.dashboard.alerts_limit_default);
    cfg.dashboard.events_limit_max =
        read_int_or_default(d, "events_limit_max", cfg.dashboard.events_limit_max);
    cfg.dashboard.alerts_limit_max =
        read_int_or_default(d, "alerts_limit_max", cfg.dashboard.alerts_limit_max);
  }

  if (cfg.server.port <= 0) cfg.server.port = 8080;
  if (cfg.dashboard.events_limit_default <= 0) cfg.dashboard.events_limit_default = 100;
  if (cfg.dashboard.alerts_limit_default <= 0) cfg.dashboard.alerts_limit_default = 100;
  if (cfg.dashboard.events_limit_max <= 0) cfg.dashboard.events_limit_max = 1000;
  if (cfg.dashboard.alerts_limit_max <= 0) cfg.dashboard.alerts_limit_max = 1000;

  return cfg;
}
