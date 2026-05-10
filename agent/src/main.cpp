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
#include <algorithm>
#include <iomanip>
#include <regex>
#include <sstream>

#include "json.hpp"
#include "http_client.hpp"
#include "log_reader.hpp"
#include "state_store.hpp"
#include "auth_parser.hpp"
#include "process_snapshot.hpp"
#include "file_monitor.hpp"
#include "config.hpp"
#include "syslog_parser.hpp"

using json = nlohmann::json;

static std::string source_name(const std::string& path);

struct ReceiverConfig {
  std::string id;
  std::string name;
  std::string role = "receiver";
  std::string parent_id;
  std::string type = "file";
  std::string path;
  std::string source_type = "linux_log";
  std::string initial_position = "end";
  std::string file_pattern = "*.log";
  bool recursive = false;
  bool enabled = true;
  json policy_group_ids = json::array();
  json dashboard_ids = json::array();
};

struct ReceiverFile {
  ReceiverConfig receiver;
  std::string path;
};

struct PendingLogLine {
  ReceiverConfig receiver;
  std::string path;
  LogLine line;
  std::string parsed_ts;
  std::string sort_key;
};

static std::uint64_t get_file_offset(const AgentState& state, const std::string& path);
static void set_file_offset(AgentState& state, const std::string& path, std::uint64_t offset);

static bool read_json_file_safe(const std::string& path, json& out) {
  try {
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open()) return false;
    in >> out;
    return true;
  } catch (...) {
    return false;
  }
}

static bool has_file_offset(const AgentState& state, const std::string& key) {
  return state.file_offsets.find(key) != state.file_offsets.end();
}

static std::uint64_t file_size_or_zero(const std::string& path) {
  try {
    if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path)) {
      return 0;
    }

    return static_cast<std::uint64_t>(std::filesystem::file_size(path));
  } catch (...) {
    return 0;
  }
}

static std::string glob_to_regex(const std::string& glob) {
  std::string out = "^";

  for (char c : glob) {
    switch (c) {
      case '*':
        out += ".*";
        break;
      case '?':
        out += ".";
        break;
      case '.':
      case '+':
      case '(':
      case ')':
      case '[':
      case ']':
      case '{':
      case '}':
      case '^':
      case '$':
      case '|':
      case '\\':
        out += "\\";
        out += c;
        break;
      default:
        out += c;
        break;
    }
  }

  out += "$";
  return out;
}

static bool filename_matches_pattern(const std::string& filename,
                                     const std::string& pattern) {
  const std::string p = pattern.empty() ? "*.log" : pattern;

  try {
    return std::regex_match(filename, std::regex(glob_to_regex(p)));
  } catch (...) {
    return filename == p;
  }
}

static std::vector<std::string> list_directory_log_files(const std::string& directory,
                                                        const std::string& pattern,
                                                        bool recursive) {
  std::vector<std::string> files;

  try {
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
      return files;
    }

    if (recursive) {
      for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (!entry.is_regular_file()) continue;

        const std::string filename = entry.path().filename().string();
        if (!filename_matches_pattern(filename, pattern)) continue;

        files.push_back(entry.path().string());
      }
    } else {
      for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (!entry.is_regular_file()) continue;

        const std::string filename = entry.path().filename().string();
        if (!filename_matches_pattern(filename, pattern)) continue;

        files.push_back(entry.path().string());
      }
    }

    std::sort(files.begin(), files.end());
  } catch (...) {
    files.clear();
  }

  return files;
}

static std::vector<ReceiverFile> expand_receiver_files(const std::vector<ReceiverConfig>& receivers) {
  std::vector<ReceiverFile> out;

  for (const auto& receiver : receivers) {
    if (!receiver.enabled) continue;

    if (receiver.type == "file") {
      if (!receiver.path.empty()) {
        out.push_back({receiver, receiver.path});
      }

      continue;
    }

    if (receiver.type == "directory") {
      auto files = list_directory_log_files(
        receiver.path,
        receiver.file_pattern.empty() ? "*.log" : receiver.file_pattern,
        receiver.recursive
      );

      for (const auto& file_path : files) {
        out.push_back({receiver, file_path});
      }

      continue;
    }
  }

  std::sort(out.begin(), out.end(), [](const ReceiverFile& a, const ReceiverFile& b) {
    if (a.receiver.id != b.receiver.id) {
      return a.receiver.id < b.receiver.id;
    }

    return a.path < b.path;
  });

  return out;
}

static std::string receiver_offset_key(const ReceiverConfig& receiver,
                                       const std::string& file_path) {
  return receiver.id + "|" + file_path;
}

static std::uint64_t get_initial_offset_for_receiver_file(AgentState& state,
                                                          const ReceiverConfig& receiver,
                                                          const std::string& file_path) {
  const std::string key = receiver_offset_key(receiver, file_path);

  if (has_file_offset(state, key)) {
    return get_file_offset(state, key);
  }

  if (receiver.initial_position == "end") {
    const std::uint64_t size = file_size_or_zero(file_path);
    set_file_offset(state, key, size);
    return size;
  }

  set_file_offset(state, key, 0);
  return 0;
}

static int month_number_from_name(const std::string& mon) {
  static const std::unordered_map<std::string, int> months = {
    {"Jan", 1}, {"Feb", 2}, {"Mar", 3}, {"Apr", 4},
    {"May", 5}, {"Jun", 6}, {"Jul", 7}, {"Aug", 8},
    {"Sep", 9}, {"Oct", 10}, {"Nov", 11}, {"Dec", 12}
  };

  auto it = months.find(mon);
  if (it == months.end()) return 0;
  return it->second;
}

static int current_utc_year() {
  std::time_t t = std::time(nullptr);
  std::tm tm{};
  gmtime_r(&t, &tm);
  return tm.tm_year + 1900;
}

static std::string make_iso_utc_no_ms(int year,
                                      int month,
                                      int day,
                                      int hour,
                                      int minute,
                                      int second) {
  std::ostringstream ss;
  ss << std::setfill('0')
     << std::setw(4) << year << "-"
     << std::setw(2) << month << "-"
     << std::setw(2) << day << "T"
     << std::setw(2) << hour << ":"
     << std::setw(2) << minute << ":"
     << std::setw(2) << second << ".000Z";

  return ss.str();
}

static bool extract_log_timestamp(const std::string& raw, std::string& out_iso) {
  // ISO/syslog-ng/journald style:
  // 2026-05-10T09:30:57.929742+00:00 smth ...
  // 2026-05-10T09:30:57Z smth ...
  static const std::regex iso_re(
    R"(^(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})(?:\.(\d+))?(?:Z|[+-]\d{2}:\d{2})?)"
  );

  std::smatch m;
  if (std::regex_search(raw, m, iso_re)) {
    const std::string year = m[1].str();
    const std::string month = m[2].str();
    const std::string day = m[3].str();
    const std::string hour = m[4].str();
    const std::string minute = m[5].str();
    const std::string second = m[6].str();

    std::string ms = "000";
    if (m.size() > 7 && m[7].matched) {
      ms = m[7].str().substr(0, 3);
      while (ms.size() < 3) ms += "0";
    }

    out_iso = year + "-" + month + "-" + day + "T" +
              hour + ":" + minute + ":" + second + "." + ms + "Z";
    return true;
  }

  // Classic syslog:
  // May 10 12:34:56 host program: message
  static const std::regex syslog_re(
    R"(^([A-Z][a-z]{2})\s+(\d{1,2})\s+(\d{2}):(\d{2}):(\d{2}))"
  );

  if (std::regex_search(raw, m, syslog_re)) {
    const int month = month_number_from_name(m[1].str());
    const int day = std::stoi(m[2].str());
    const int hour = std::stoi(m[3].str());
    const int minute = std::stoi(m[4].str());
    const int second = std::stoi(m[5].str());

    if (month > 0) {
      out_iso = make_iso_utc_no_ms(
        current_utc_year(),
        month,
        day,
        hour,
        minute,
        second
      );
      return true;
    }
  }

  return false;
}

static std::string make_sort_key(const std::string& parsed_ts,
                                 const std::string& file_path,
                                 std::uint64_t next_offset,
                                 std::uint64_t fallback_seq) {
  std::ostringstream ss;

  if (!parsed_ts.empty()) {
    ss << parsed_ts;
  } else {
    ss << "9999-12-31T23:59:59.999Z";
  }

  ss << "|"
     << file_path
     << "|"
     << std::setw(20) << std::setfill('0') << next_offset
     << "|"
     << std::setw(20) << std::setfill('0') << fallback_seq;

  return ss.str();
}

static std::vector<ReceiverConfig> load_receivers_from_file(const std::string& path,
                                                            const AppConfig& config) {
  std::vector<ReceiverConfig> receivers;

  json arr;
  if (read_json_file_safe(path, arr) && arr.is_array()) {
    for (const auto& item : arr) {
      if (!item.is_object()) continue;
      if (item.value("enabled", true) == false) continue;

      ReceiverConfig r;
      r.id = item.value("id", "");
      r.name = item.value("name", r.id);
      r.role = item.value("role", "receiver");
      r.parent_id = item.value("parent_id", "");
      r.type = item.value("type", "file");
      r.path = item.value("path", "");
      r.source_type = item.value("source_type", "linux_log");
      r.initial_position = item.value("initial_position", "end");
      r.file_pattern = item.value("file_pattern", "*.log");
      r.recursive = item.value("recursive", false);

      if (item.contains("policy_group_ids") && item["policy_group_ids"].is_array()) {
        r.policy_group_ids = item["policy_group_ids"];
      }

      if (item.contains("dashboard_ids") && item["dashboard_ids"].is_array()) {
 	 r.dashboard_ids = item["dashboard_ids"];
      }

      if (!r.id.empty() && item.value("enabled", true) != false) {
 	 if ((r.type == "file" || r.type == "directory") && !r.path.empty()) {
   	   receivers.push_back(std::move(r));
 	 }
      }

    }
  }

  if (!receivers.empty()) {
    return receivers;
  }

  // fallback на старый config.agent.log_paths
  int i = 0;
  for (const auto& log_path : config.agent.log_paths) {
    ReceiverConfig r;
    r.id = "legacy_recv_" + std::to_string(++i);
    r.name = source_name(log_path);
    r.type = "file";
    r.path = log_path;
    r.source_type = "linux_log";
    r.initial_position = "end";
    r.file_pattern = "*.log";
    r.recursive = false;
    r.policy_group_ids = json::array();

    receivers.push_back(std::move(r));
  }

  return receivers;
}

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
    // Do not let logging crash the agent.
  }
}

static void log_agent_info(const std::string& log_path, const std::string& message) {
  append_agent_log(log_path, AgentLogLevel::Info, message);
}

static void log_agent_warn(const std::string& log_path, const std::string& message) {
  append_agent_log(log_path, AgentLogLevel::Warn, message);
}

static void log_agent_error(const std::string& log_path, const std::string& message) {
  append_agent_log(log_path, AgentLogLevel::Error, message);
}

static std::string source_name(const std::string& path) {
  return std::filesystem::path(path).filename().string();
}

static bool is_auth_log_source(const std::string& path) {
  return path.find("auth") != std::string::npos ||
         path.find("demo_auth") != std::string::npos;
}

static bool can_open_file_for_reading(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  return in.is_open();
}

static std::uint64_t get_file_offset(const AgentState& state, const std::string& path) {
  auto it = state.file_offsets.find(path);
  if (it == state.file_offsets.end()) {
    return 0;
  }
  return it->second;
}

static void set_file_offset(AgentState& state,
                            const std::string& path,
                            std::uint64_t offset) {
  state.file_offsets[path] = offset;
}

int main() {
  try {
    std::filesystem::create_directories("logs");
    std::filesystem::create_directories("data");

    const AppConfig config = ConfigLoader::load_from_file("config/config.json");

    HttpClient client(config.server.host, config.server.port);

    const char* agent_token_env = std::getenv("MINI_SIEM_AGENT_TOKEN");
    client.set_bearer_token(agent_token_env ? std::string(agent_token_env) : "dev-agent-token");

    const std::string host = get_hostname_safe();
    const std::string ssh_watch_path = config.agent.ssh_watch_path;
    const std::string state_path = config.agent.state_path;
    const std::string agent_log_path = config.agent.agent_log_path;

    StateStore state_store(state_path);
//    AuthParser auth_parser;
    ProcessSnapshot process_snapshot;
    FileMonitor file_monitor;

    AgentState state = state_store.load();

//    SyslogParser syslog_parser;

    std::unordered_set<int> known_pids;
    for (const auto& proc : state.known_processes) {
      known_pids.insert(proc.pid);
    }

    bool process_baseline_initialized = !state.known_processes.empty();

    std::unordered_map<std::string, bool> missing_log_reported;
    std::unordered_map<std::string, bool> unreadable_log_reported;

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

    std::cout << "[mini-siem-agent] started\n";
    std::cout << "[mini-siem-agent] log sources:\n";

    log_agent_info(agent_log_path, "agent started");
    log_agent_info(agent_log_path, "configured log sources:");

    auto receivers = load_receivers_from_file("config/receivers.json", config);

    for (const auto& receiver : receivers) {
      std::cout << "  - " << receiver.id << " " << receiver.path << "\n";
      log_agent_info(agent_log_path, "receiver=" + receiver.id + " path=" + receiver.path);
    }

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

/*      // --------- multiple log source monitoring ---------
      for (const auto& log_path : config.agent.log_paths) {
        LogReader reader(log_path);

        if (!reader.exists()) {
          if (!missing_log_reported[log_path]) {
            log_agent_warn(agent_log_path, "log file not found: " + log_path);
            missing_log_reported[log_path] = true;
          }
          continue;
        }

        if (missing_log_reported[log_path]) {
          log_agent_info(agent_log_path, "log file became available again: " + log_path);
          missing_log_reported[log_path] = false;
        }

        if (!can_open_file_for_reading(log_path)) {
          if (!unreadable_log_reported[log_path]) {
            log_agent_warn(agent_log_path, "log file exists but cannot be opened for reading: " + log_path);
            unreadable_log_reported[log_path] = true;
          }
          continue;
        }

        if (unreadable_log_reported[log_path]) {
          log_agent_info(agent_log_path, "log file became readable again: " + log_path);
          unreadable_log_reported[log_path] = false;
        }

        const std::uint64_t offset = get_file_offset(state, log_path);
        auto lines = reader.read_new_lines(offset, 100);

        for (auto& item : lines) {
          json event;
          event["ts"] = now_iso_utc();
          event["host"] = host;
          event["source"] = source_name(log_path);
          event["log_path"] = log_path;
          event["raw"] = item.text;

          if (is_auth_log_source(log_path)) {
            auto parsed = auth_parser.parse(item.text);

            if (parsed.has_value()) {
              event["event_type"] = parsed->event_type;
              event["severity"] = parsed->severity;

              for (auto it = parsed->fields.begin(); it != parsed->fields.end(); ++it) {
                event[it.key()] = it.value();
              }
            } else {
              event["event_type"] = "raw_log_line";
              event["severity"] = "info";
            }
	  } else {
  	    auto parsed_syslog = syslog_parser.parse(item.text, source_name(log_path));

  	    for (auto it = parsed_syslog.begin(); it != parsed_syslog.end(); ++it) {
   	      event[it.key()] = it.value();
 	    }
	  }

          std::string response_text;
          bool ok = client.post_json("/ingest", event.dump(), response_text);

          if (ok) {
            set_file_offset(state, log_path, item.next_offset);
            state_store.save(state);

            std::string msg = "[OK] sent source=" +
                              source_name(log_path) +
                              " event_type=" +
                              event["event_type"].get<std::string>() +
                              " offset=" +
                              std::to_string(item.next_offset) +
                              " raw=" + item.text;

            std::cout << msg << "\n";
            log_agent_info(agent_log_path, msg);
          } else {
            std::string msg = "[ERR] failed to send source=" +
                              source_name(log_path) +
                              " event_type=" +
                              event["event_type"].get<std::string>() +
                              " response=" + response_text +
                              " raw=" + item.text;

            std::cerr << msg << "\n";
            log_agent_error(agent_log_path, msg);
            break;
          }
        }
      }
*/

      // --------- receiver-based raw log monitoring ---------
      receivers = load_receivers_from_file("config/receivers.json", config);

      const auto receiver_files = expand_receiver_files(receivers);

      std::vector<PendingLogLine> pending_lines;
      std::uint64_t fallback_seq = 0;

      for (const auto& rf : receiver_files) {
        const ReceiverConfig& receiver = rf.receiver;
        const std::string log_path = rf.path;
        const std::string offset_key = receiver_offset_key(receiver, log_path);

        LogReader reader(log_path);

        if (!reader.exists()) {
          if (!missing_log_reported[offset_key]) {
            log_agent_warn(agent_log_path,
                           "receiver log file not found: receiver=" +
                           receiver.id +
                           " path=" +
                           log_path);
            missing_log_reported[offset_key] = true;
          }

          continue;
        }

        if (missing_log_reported[offset_key]) {
          log_agent_info(agent_log_path,
                         "receiver log file became available again: receiver=" +
                         receiver.id +
                         " path=" +
                         log_path);
          missing_log_reported[offset_key] = false;
        }

        if (!can_open_file_for_reading(log_path)) {
          if (!unreadable_log_reported[offset_key]) {
            log_agent_warn(agent_log_path,
                           "receiver log file exists but cannot be opened: receiver=" +
                           receiver.id +
                           " path=" +
                           log_path);
            unreadable_log_reported[offset_key] = true;
          }

          continue;
        }

        if (unreadable_log_reported[offset_key]) {
          log_agent_info(agent_log_path,
                         "receiver log file became readable again: receiver=" +
                         receiver.id +
                         " path=" +
                         log_path);
          unreadable_log_reported[offset_key] = false;
        }

        std::uint64_t offset = get_initial_offset_for_receiver_file(
          state,
          receiver,
          log_path
        );

        const std::uint64_t current_size = file_size_or_zero(log_path);

        if (offset > current_size) {
          log_agent_warn(agent_log_path,
                         "receiver log file was truncated or rotated: receiver=" +
                         receiver.id +
                         " path=" +
                         log_path +
                         " old_offset=" +
                         std::to_string(offset) +
                         " new_size=" +
                         std::to_string(current_size));

          offset = 0;
          set_file_offset(state, offset_key, 0);
        }

        auto lines = reader.read_new_lines(offset, 100);

        for (auto& line : lines) {
          std::string parsed_ts;
          extract_log_timestamp(line.text, parsed_ts);

          PendingLogLine pending;
          pending.receiver = receiver;
          pending.path = log_path;
          pending.line = std::move(line);
          pending.parsed_ts = parsed_ts;
          pending.sort_key = make_sort_key(
            parsed_ts,
            log_path,
            pending.line.next_offset,
            ++fallback_seq
          );

          pending_lines.push_back(std::move(pending));
        }
      }

      std::sort(
        pending_lines.begin(),
        pending_lines.end(),
        [](const PendingLogLine& a, const PendingLogLine& b) {
          return a.sort_key < b.sort_key;
        }
      );

      for (const auto& pending : pending_lines) {
        const ReceiverConfig& receiver = pending.receiver;
        const std::string& log_path = pending.path;
        const std::string offset_key = receiver_offset_key(receiver, log_path);

        json event;

        if (!pending.parsed_ts.empty()) {
          event["ts"] = pending.parsed_ts;
          event["log_ts"] = pending.parsed_ts;
        } else {
          event["ts"] = now_iso_utc();
        }

        event["host"] = host;

        event["receiver_id"] = receiver.id;
        event["receiver_name"] = receiver.name;
        event["receiver_type"] = receiver.type;
        event["receiver_role"] = receiver.role;
        event["receiver_parent_id"] = receiver.parent_id;

        event["policy_group_ids"] = receiver.policy_group_ids;
        event["dashboard_ids"] = receiver.dashboard_ids;

        event["source"] = source_name(log_path);
        event["source_path"] = log_path;
        event["log_path"] = log_path;
        event["source_type"] = receiver.source_type;

        event["event_type"] = "unknown_event";
        event["event_name"] = "Unknown Event";
        event["parser_status"] = "unparsed";
        event["severity"] = "info";
        event["raw"] = pending.line.text;

        if (receiver.type == "directory") {
          event["directory_receiver_path"] = receiver.path;
          event["directory_file_pattern"] = receiver.file_pattern;
          event["directory_recursive"] = receiver.recursive;
        }

        std::string response_text;
        bool ok = client.post_json("/ingest", event.dump(), response_text);

        if (ok) {
          set_file_offset(state, offset_key, pending.line.next_offset);
          state_store.save(state);

          std::string msg =
            "[OK] sent receiver=" +
            receiver.id +
            " type=" +
            receiver.type +
            " file=" +
            log_path +
            " parser_status=unparsed offset=" +
            std::to_string(pending.line.next_offset) +
            " ts=" +
            event.value("ts", "") +
            " raw=" +
            pending.line.text;

          std::cout << msg << "\n";
          log_agent_info(agent_log_path, msg);
        } else {
          std::string msg =
            "[ERR] failed to send receiver=" +
            receiver.id +
            " type=" +
            receiver.type +
            " file=" +
            log_path +
            " response=" +
            response_text +
            " raw=" +
            pending.line.text;

          std::cerr << msg << "\n";
          log_agent_error(agent_log_path, msg);
          break;
        }
      }

      state_store.save(state);


      state_store.save(state);
      std::this_thread::sleep_for(std::chrono::seconds(2));
    }

  } catch (const std::exception& e) {
    std::cerr << "Fatal: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
