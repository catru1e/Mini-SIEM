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

#include "httplib.h"
#include "json.hpp"
#include "db_sqlite.hpp"
#include "detect.hpp"
#include "correlate.hpp"
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

static std::string safe_string(const json& j, const char* key, const std::string& def = "") {
  if (!j.contains(key) || j[key].is_null()) return def;
  if (j[key].is_string()) return j[key].get<std::string>();
  // если не строка — сериализуем в строку
  return j[key].dump();
}

static bool read_file(const std::string& path, std::string& out) {
  std::ifstream file(path, std::ios::binary);
  if (!file) return false;

  std::ostringstream ss;
  ss << file.rdbuf();
  out = ss.str();
  return true;
}

static json make_event_response_item(long long id, const json& original) {
  return json {
    {"id", id},
    {"ts", safe_string(original, "ts", "")},
    {"received_at", now_iso_utc()},
    {"host", safe_string(original, "host", "unknown")},
    {"event_type", safe_string(original, "event_type", "unknown")},
    {"source", safe_string(original, "source", "unknown")},
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
  if (requested <- 0) return def;
  if (requested > max_value) return max_value;
  return requested;
}

struct SseClient {
  std::mutex mutex;
  std::deque<std::string> queue;
  bool active = true;
};

int main() {
  try {
    // Важно: относительный путь => работает с флешки
    std::filesystem::create_directories("data");
    std::filesystem::create_directories("logs");
    std::filesystem::create_directories("web");

    const AppConfig config = ConfigLoader::load_from_file("config/config.json");

    SqliteDb db(config.server.db_path);
    db.init();
    DetectionEngine detector(db, config.detection);
    CorrelationEngine correlator(db);

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

    // Healthcheck
    srv.Get("/health", [&](const httplib::Request&, httplib::Response& res) {
      res.set_content("OK\n", "text/plain");
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

    srv.Get("/stream", [&](const httplib::Request&, httplib::Response& res){
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

    // Ingest endpoint
    srv.Post("/ingest", [&](const httplib::Request& req, httplib::Response& res) {
      try {
        auto j = json::parse(req.body);

        // Заполняем минимальные поля, если агент не прислал
        std::string ts = safe_string(j, "ts", now_iso_utc());
        std::string event_type = safe_string(j, "event_type", "unknown");
        std::string source = safe_string(j, "source", "unknown");

	if (!j.contains("ts")) j["ts"] = ts;
	if (!j.contains("event_type")) j["event_type"] = event_type;
	if (!j.contains("source")) j["source"] = source;

        // Канонизируем json (храним строкой)
        std::string body = j.dump();

        db.insert_event(ts, event_type, source, body);

	broadcast_sse("event", make_event_response_item(0, j));

	try {
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

        json out = {{"status", "ok"}};
        res.set_content(out.dump(), "application/json");
        res.status = 200;
      } catch (const std::exception& e) {
	std::cerr << "[ingest][ERR] " << e.what() << "\n";
        json out = {{"status", "error"}, {"message", e.what()}};
        res.set_content(out.dump(), "application/json");
        res.status = 400;
      }
    });

    // Get last events
    srv.Get("/api/events", [&](const httplib::Request& req, httplib::Response& res) {
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
          {"ts", r.ts},
	  {"received_at", r.received_at},
	  {"host", r.host},
          {"event_type", r.event_type},
          {"source", r.source},
	  {"severity", r.severity},
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
