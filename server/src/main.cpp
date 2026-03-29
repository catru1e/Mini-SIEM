#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "httplib.h"
#include "json.hpp"
#include "db_sqlite.hpp"
#include "detect.hpp"
#include "correlate.hpp"

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

int main() {
  try {
    // Важно: относительный путь => работает с флешки
    std::filesystem::create_directories("data");
    std::filesystem::create_directories("logs");
    std::filesystem::create_directories("web");

    SqliteDb db("data/events.db");
    db.init();
    DetectionEngine detector(db);
    CorrelationEngine correlator(db);

    httplib::Server srv;

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

    // Ingest endpoint
    srv.Post("/ingest", [&](const httplib::Request& req, httplib::Response& res) {
      try {
        auto j = json::parse(req.body);

        // Заполняем минимальные поля, если агент не прислал
        std::string ts = safe_string(j, "ts", now_iso_utc());
        std::string event_type = safe_string(j, "event_type", "unknown");
        std::string source = safe_string(j, "source", "unknown");

        // Канонизируем json (храним строкой)
        std::string body = j.dump();

        db.insert_event(ts, event_type, source, body);

	try {
	  detector.process_event(j);
	  correlator.process_event(j);
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
      int limit = 100;
      if (req.has_param("limit")) {
        try {
          limit = std::stoi(req.get_param_value("limit"));
        } catch (...) {
          limit = 100;
        }
      }

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
      int limit = 100;
      if (req.has_param("limit")) {
        try {
          limit = std::stoi(req.get_param_value("limit"));
        } catch (...) {
          limit = 100;
        }
      }

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

    std::cout << "[mini-siem-server] listening on http://127.0.0.1:8080\n";
    srv.listen("127.0.0.1", 8080);
  } catch (const std::exception& e) {
    std::cerr << "Fatal: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
