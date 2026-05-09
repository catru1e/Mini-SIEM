#pragma once

#include <string>
#include <vector>

struct PathsConfig {
  std::string data_dir = "data";
  std::string logs_dir = "logs";
};

struct ServerConfig {
  std::string host = "127.0.0.1";
  int port = 8080;
  std::string db_path = "data/events.db";
};

struct AgentConfig {
  std::vector<std::string> log_paths = {
    "logs/demo_auth.log",
    "/var/log/auth.log",
    "/var/log/syslog",
    "/var/log/kern.log"
  };
  std::string state_path = "data/agent_state.json";
  std::string agent_log_path = "logs/agent.log";
  std::string ssh_watch_path = "~/.ssh";
};

struct DetectionConfig {
  int failed_login_threshold = 5;
  int failed_login_window_seconds = 300;

  int invalid_user_threshold = 5;
  int invalid_user_window_seconds = 300;

  int sudo_threshold = 3;
  int sudo_window_seconds = 300;

  int success_login_threshold = 10;
  int success_login_window_seconds = 120;

  std::vector<std::string> blacklisted_processes = {
    "nc",
    "netcat",
    "ncat",
    "socat"
  };
};

struct DashboardConfig {
  int events_limit_default = 10000;
  int alerts_limit_default = 10000;

  int events_limit_max = 0;
  int alerts_limit_max = 0;
};

struct AuthConfig {
  std::string db_path = "data/auth.db";
  std::string session_cookie = "mini_siem_session";
  int session_ttl_seconds = 86400;
};

struct AppConfig {
  ServerConfig server;
  AgentConfig agent;
  DetectionConfig detection;
  DashboardConfig dashboard;
  AuthConfig auth;
  PathsConfig paths;
};

class ConfigLoader{
public:
  static AppConfig load_from_file(const std::string& path);
};
