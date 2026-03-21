#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>
#include <filesystem>
#include <fstream>
#include <unistd.h>

#include "json.hpp"
#include "http_client.hpp"
#include "log_reader.hpp"
#include "state_store.hpp"
#include "auth_parser.hpp"

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

static void append_agent_log(const std::string& line) {
  std::filesystem::create_directories("logs");
  std::ofstream out("logs/agent.log", std::ios::app);
  out << line << "\n";
}

static std::string choose_log_source() {
  if (std::filesystem::exists("logs/demo_auth.log"))
    return "logs/demo_auth.log";

  if (std::filesystem::exists("/var/log/auth.log"))
    return "/var/log/auth.log";

  return "logs/demo_auth.log";
}

static std::string source_name(const std::string& path) {
  return std::filesystem::path(path).filename().string();
}

int main() {
  try {
    std::filesystem::create_directories("logs");
    std::filesystem::create_directories("data");

    HttpClient client("127.0.0.1", 8080);

    const std::string host = get_hostname_safe();
    const std::string log_path = choose_log_source();

    LogReader reader(log_path);
    StateStore state_store("data/agent_state.json");
    AuthParser auth_parser;

    AgentState state = state_store.load();

    if (state.log_path != log_path) {
      state.log_path = log_path;
      state.offset = 0;
      state_store.save(state);
    }

    std::cout << "[mini-siem-agent] started, reading log: " << log_path << "\n";
    append_agent_log("agent started, source=" + log_path);

    while (true) {

      if (!reader.exists()) {
        append_agent_log("log file not found yet: " + log_path);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        continue;
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
          append_agent_log(msg);
        }
        else {
	  std::string msg = "[ERR] failed to send event_type=" +
			    event["event_type"].get<std::string>() =
			    " raw=" + item.text;

          std::cerr << msg << "\n";
          append_agent_log(msg);
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
