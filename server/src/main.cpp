#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <filesystem>

#include "httplib.h"
#include "json.hpp"
#include "db_sqlite.hpp"

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

int main() {
  try {
    // Важно: относительный путь => работает с флешки
    std::filesystem::create_directories("data");
    std::filesystem::create_directories("logs");

    SqliteDb db("data/events.db");
    db.init();

    httplib::Server srv;

    // Healthcheck
    srv.Get("/health", [&](const httplib::Request&, httplib::Response& res) {
      res.set_content("OK\n", "text/plain");
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

        json out = {{"status", "ok"}};
        res.set_content(out.dump(), "application/json");
        res.status = 200;
      } catch (const std::exception& e) {
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
          {"event_type", r.event_type},
          {"source", r.source},
          {"event", original}
        };
        arr.push_back(std::move(item));
      }

      json out = {{"events", arr}};
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
