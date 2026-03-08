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

int main() {
  try {
    std::filesystem::create_directories("logs");

    HttpClient client("127.0.0.1", 8080);
    const std::string host = get_hostname_safe();

    std::cout << "[mini-siem-agent] started, sending demo events to http://127.0.0.1:8080/ingest\n";

    int counter = 0;

    while (true) {
      json event = {
        {"ts", now_iso_utc()},
        {"host", host},
        {"source", "agent_demo"},
        {"event_type", "heartbeat"},
        {"severity", "info"},
        {"message", "demo event from agent"},
        {"counter", counter}
      };

      std::string response_text;
      bool ok = client.post_json("/ingest", event.dump(), response_text);

      if (ok) {
        std::string msg = "[OK] sent event counter=" + std::to_string(counter) + " response=" + response_text;
        std::cout << msg << "\n";
        append_agent_log(msg);
      } else {
        std::string msg = "[ERR] failed to send event counter=" + std::to_string(counter) + " response=" + response_text;
        std::cerr << msg << "\n";
        append_agent_log(msg);
      }

      counter++;
      std::this_thread::sleep_for(std::chrono::seconds(2));
    }
  } catch (const std::exception& e) {
    std::cerr << "Fatal: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
