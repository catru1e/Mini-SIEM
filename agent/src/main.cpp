#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>
#include <filesystem>
#include <fstream>
#include <unistd.h>
#include <unordered_set>
#include <unordered_map>
#include <cstdlib>

#include "json.hpp"
#include "http_client.hpp"
#include "log_reader.hpp"
#include "state_store.hpp"
#include "auth_parser.hpp"
#include "process_snapshot.hpp"
#include "file_monitor.hpp"
#include "config.hpp"

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

static std::string get_hostname_safe() {
  char buf[256];
  if (gethostname(buf, sizeof(buf)) == 0) {
    buf[sizeof(buf) - 1] = '\0';
    return std::string(buf);
  }
  return "unknown-host";
}


//updated and better
enum class AgentLogLevel {
  Info,
  Warn,
  Error
};

static const char* to_string(AgentLogLevel level) {
  switch (level) {
    case AgentLogLevel::Info:  return "INFO";
    case AgentLogLevel::Warn:  return "WARN";
    case AgentLogLevel::Error: return "ERROR";
  }
  return "INFO";
}

static void append_agent_log(const std::string& log_path,
			     AgentLogLevel level,
			     const std::string& message) {
  if (log_path.empty()) return;
  if (message.empty()) return;
  try {
    std::filesystem::path p(log_path);

    if (p.filename().empty()) return;

    if (p.has_parent_path()) {
      std::filesystem::create_directories(p.parent_path());
    }

    std::ofstream out(log_path, std::ios::app);
    if (!out.is_open()) {
      return;
    }

    out << now_iso_utc()
	<< " [" << to_string(level) << "] "
	<< message << "\n";
  } catch (...) {
    // chto bi agent ne upal
  }
}

static void log_agent_info(const std::string& log_path, const std::string& message) {
  append_agent_log(log_path, AgentLogLevel::Info, message);
}
static void log_agent_warn(const std::string& log_path, const std::string& message){
  append_agent_log(log_path, AgentLogLevel::Warn, message);
}
static void log_agent_error(const std::string& log_path, const std::string& message) {
  append_agent_log(log_path, AgentLogLevel::Error, message);
}

static std::string choose_log_source(const std::vector<std::string>& log_paths) {
  for (const auto& path : log_paths) {
    if (std::filesystem::exists(path)) {
      return path;
    }
  }
  if (!log_paths.empty()) {
    return log_paths.front();
  }
  return "logs/demo_auth.log";
}

static std::string source_name(const std::string& path) {
  return std::filesystem::path(path).filename().string();
}

//dlya inotify 
static std::string choose_ssh_watch_path() {
  const char* home = std::getenv("HOME");
  if (!home || std::string(home).empty()){
    return ".ssh";
  }
  return std::string(home) + "/.ssh";
}

int main() {
  try {
    std::filesystem::create_directories("logs");
    std::filesystem::create_directories("data");

    const AppConfig config = ConfigLoader::load_from_file("config/config.json");

    HttpClient client(config.server.host, config.server.port);

    const std::string host = get_hostname_safe();
    const std::string log_path = choose_log_source(config.agent.log_paths);
    const std::string ssh_watch_path = config.agent.ssh_watch_path;
    const std::string state_path = config.agent.state_path;
    const std::string agent_log_path = config.agent.agent_log_path;

    LogReader reader(log_path);
    StateStore state_store(state_path);
    AuthParser auth_parser;
    ProcessSnapshot process_snapshot;
    FileMonitor file_monitor;

    AgentState state = state_store.load();
    std::unordered_set<int> known_pids;
    for (const auto& proc : state.known_processes) {
      known_pids.insert(proc.pid);
    }

    if (state.log_path != log_path) {
      state.log_path = log_path;
      state.offset = 0;
      state_store.save(state);
    }

    bool process_baseline_initialized = !state.known_processes.empty();
    bool log_missing_reported = false;
    bool log_recovered_reported = false;

    bool file_monitor_ready = false;
    if (std::filesystem::exists(ssh_watch_path) && std::filesystem::is_directory(ssh_watch_path)) {
      file_monitor_ready = file_monitor.init(ssh_watch_path);
      if (file_monitor_ready) {
  	log_agent_info(agent_log_path, "file monitor started, path=" + ssh_watch_path);
      } else {
  	log_agent_warn(agent_log_path, "file monitor failed to start, path=" + ssh_watch_path);
      }
    } else {
      log_agent_warn(agent_log_path, "file monitor path not found: " + ssh_watch_path);
    }

    std::cout << "[mini-siem-agent] started, reading log: " << log_path << "\n";
    log_agent_info(agent_log_path, "agent started, source=" + log_path);

    while (true) {
      // ------- process snapshot monitoring -------
      {
	auto processes = process_snapshot.capture();
	std::unordered_set<int> current_pids;
	std::vector<ProcessStateEntry> new_state_processes;

	for (const auto& proc : processes) {
	  if (proc.process_name == "(udev-worker)") {
  	    continue;
	  }
	  current_pids.insert(proc.pid);
	  new_state_processes.push_back({proc.pid, proc.process_name});

	  if (known_pids.find(proc.pid) == known_pids.end()) {
	    json event;
	    event["ts"] = now_iso_utc();
	    event["host"] = host;
	    event["source"] = "proc";
	    event["event_type"] = "process_start";
	    event["severity"] = "info";
	    event["pid"] = proc.pid;
	    event["process_name"] = proc.process_name;
	    event["cmdline"] = proc.cmdline;
	    event["raw"] = "new process detected: " + proc.process_name +
			   " pid=" + std::to_string(proc.pid);

	    std::string response_text;
	    bool ok = client.post_json("/ingest", event.dump(), response_text);

	    if (ok) {
	      std::string msg = "[OK] sent process_start pid=" +
				std::to_string(proc.pid) +
				" process_name=" + proc.process_name;
	      std::cout << msg << "\n";
	      log_agent_info(agent_log_path, msg);
	    } else {
	      std::string msg = "[ERR] failed to send process_start pid=" +
				std::to_string(proc.pid) +
				" response=" + response_text;
	      std::cerr << msg << "\n";
	      log_agent_error(agent_log_path, msg);
	    }
	  }
	}
	known_pids = std::move(current_pids);
	state.known_processes = std::move(new_state_processes);
	state_store.save(state);

	if (!process_baseline_initialized) {
	  process_baseline_initialized = true;
	  log_agent_info(agent_log_path, "process baseline initialized");
	}
      }

      // --------- file monitor (~/.ssh) ---------
      if (file_monitor_ready) {
	auto file_events = file_monitor.poll_events();

	for (const auto& fe : file_events) {
	  json event;
	  event["ts"] = now_iso_utc();
	  event["host"] = host;
	  event["source"] = "inotify:ssh";
	  event["event_type"] = fe.event_type;
	  event["severity"] = "medium";
	  event["watched_path"] = fe.watched_path;
	  event["path"] = fe.full_path;
	  event["relative_path"] = fe.relative_path;
	  event["mask"] = fe.mask;
	  event["raw"] = fe.event_type + "; " + fe.full_path;

	  std::string response_text;
	  bool ok = client.post_json("/ingest", event.dump(), response_text);

	  if (ok) {
	    std::string msg = "[OK] sent " + fe.event_type +
			      " path=" + fe.full_path;
	    std::cout << msg << "\n";
	    log_agent_info(agent_log_path, msg);
	  } else {
	    std::string msg = "[ERR] failed to send " + fe.event_type +
			      " response=" + response_text +
			      " path=" + fe.full_path;
	    std::cerr << msg << "\n";
	    log_agent_error(agent_log_path, msg);
	  }
	}
      }

      // --------- auth.log monitoring --------
      if (!reader.exists()) {
	if (!log_missing_reported) {
	  log_agent_warn(agent_log_path, "log file not found yet: " + log_path);
	  log_missing_reported = true;
	  log_recovered_reported = false;
	}
	std::this_thread::sleep_for(std::chrono::seconds(2));
	continue;
      }

      if (log_missing_reported && !log_recovered_reported) {
	log_agent_info(agent_log_path, "log file became available again: " + log_path);
	log_recovered_reported = true;
	log_missing_reported = false;
      }

      auto lines = reader.read_new_lines(state.offset, 100);

      if (lines.empty()) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        continue;
      }

      for (auto& item : lines) {
	json event;
	event["ts"] = now_iso_utc();
	event["host"] = host;
	event["source"] = source_name(log_path);

	auto parsed = auth_parser.parse(item.text);

	if (parsed.has_value()){
	  event["event_type"] = parsed->event_type;
	  event["severity"] = parsed->severity;

	  for (auto it = parsed->fields.begin(); it != parsed->fields.end(); ++it) {
	    event[it.key()] = it.value();
	  }
	} else {
	  event["event_type"] = "raw_log_line";
	  event["severity"] = "info";
	  event["raw"] = item.text;
	}

        std::string response_text;
        bool ok = client.post_json("/ingest", event.dump(), response_text);

        if (ok) {
          state.offset = item.next_offset;
          state_store.save(state);

	  std::string msg = "[OK] sent event_type=" +
			    event["event_type"].get<std::string>() +
			    " offset=" + std::to_string(state.offset) +
			    " raw=" + item.text;

          std::cout << msg << "\n";
          log_agent_info(agent_log_path, msg);
        }
        else {
	  std::string msg = "[ERR] failed to send event_type=" +
			    event["event_type"].get<std::string>() +
			    " response=" + response_text +
			    " raw=" + item.text;

          std::cerr << msg << "\n";
          log_agent_error(agent_log_path, msg);
          break;
        }
      }
    }

  } catch (const std::exception& e) {
    std::cerr << "Fatal: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
