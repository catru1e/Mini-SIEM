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
#include <random>

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

static std::string lower_copy(std::string s) {
  for (char& c : s) {
    if (c >= 'A' && c <= 'Z') {
      c = static_cast<char>(c - 'A' + 'a');
    }
  }
  return s;
}

static std::string generate_event_id() {
  static const char* hex = "0123456789abcdef";

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(0, 15);
  std::uniform_int_distribution<int> variant_dist(8, 11);

  std::string id = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";

  for (char& c : id) {
    if (c == 'x') {
      c = hex[dist(gen)];
    } else if (c == 'y') {
      c = hex[variant_dist(gen)];
    }
  }

  return id;
}

static bool is_auth_event_type(const std::string& event_type) {
  const std::string t = lower_copy(event_type);

  return t == "failed_login" ||
         t == "accepted_login" ||
         t == "invalid_user" ||
         t == "privilege_escalation" ||
         t == "session_open" ||
         t == "session_close" ||
         t == "auth_failed" ||
         t == "auth_success" ||
         t == "auth_invalid_user";
}

static std::string detect_source_type(const json& event) {
  const std::string source = lower_copy(safe_string(event, "source", ""));
  const std::string event_type = lower_copy(safe_string(event, "event_type", ""));

  if (source == "proc" || event_type == "process_start") {
    return "process";
  }

  if (source.rfind("inotify", 0) == 0 ||
      event_type.rfind("file_", 0) == 0 ||
      event.contains("watched_path")) {
    return "file";
  }

  if (source.find("auth") != std::string::npos ||
      source.find("demo_auth") != std::string::npos ||
      is_auth_event_type(event_type)) {
    return "auth";
  }

  if (source.find("syslog") != std::string::npos ||
      source.find("kern") != std::string::npos ||
      event_type == "system_event") {
    return "syslog";
  }

  return "syslog";
}

static bool read_file(const std::string& path, std::string& out) {
  std::ifstream file(path, std::ios::binary);
  if (!file) return false;

  std::ostringstream ss;
  ss << file.rdbuf();
  out = ss.str();
  return true;
}

static bool read_json_file(const std::string& path, json& out, std::string& err) {
  try {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
      err = "failed to open file: " + path;
      return false;
    }

    file >> out;
    return true;
  } catch (const std::exception& e) {
    err = e.what();
    return false;
  }
}

static bool write_json_file(const std::string& path, const json& data, std::string& err) {
  try {
    std::filesystem::path p(path);

    if (p.has_parent_path()) {
      std::filesystem::create_directories(p.parent_path());
    }

    const std::string tmp_path = path + ".tmp";

    {
      std::ofstream out(tmp_path, std::ios::binary | std::ios::trunc);
      if (!out.is_open()) {
        err = "failed to write file: " + tmp_path;
        return false;
      }

      out << data.dump(2) << "\n";
    }

    std::filesystem::rename(tmp_path, path);
    return true;
  } catch (const std::exception& e) {
    err = e.what();
    return false;
  }
}

static bool validate_rule_json(const json& rule, std::string& err) {
  if (!rule.is_object()) {
    err = "rule must be a JSON object";
    return false;
  }

  if (!rule.contains("id") || !rule["id"].is_string() || rule["id"].get<std::string>().empty()) {
    err = "rule.id must be a non-empty string";
    return false;
  }

  if (rule.contains("enabled") && !rule["enabled"].is_boolean()) {
    err = "rule.enabled must be boolean";
    return false;
  }

  if (rule.contains("type") && !rule["type"].is_string()) {
    err = "rule.type must be string";
    return false;
  }

  if (rule.contains("threshold") && !rule["threshold"].is_number_integer()) {
    err = "rule.threshold must be integer";
    return false;
  }

  if (rule.contains("window_sec") && !rule["window_sec"].is_number_integer()) {
    err = "rule.window_sec must be integer";
    return false;
  }

  if (rule.contains("suppress_sec") && !rule["suppress_sec"].is_number_integer()) {
    err = "rule.suppress_sec must be integer";
    return false;
  }

  return true;
}

static int find_rule_index_by_id(const json& rules, const std::string& id) {
  if (!rules.is_array()) return -1;

  for (std::size_t i = 0; i < rules.size(); ++i) {
    const auto& item = rules[i];

    if (!item.is_object()) continue;
    if (!item.contains("id") || !item["id"].is_string()) continue;

    if (item["id"].get<std::string>() == id) {
      return static_cast<int>(i);
    }
  }

  return -1;
}

static void send_json_response(httplib::Response& res, int status, const json& body) {
  res.status = status;
  res.set_content(body.dump(), "application/json");
}

static json make_event_response_item(long long id, const json& original) {
  return json {
    {"id", id},
    {"event_id", safe_string(original, "event_id", "")},
    {"ts", safe_string(original, "ts", "")},
    {"received_at", safe_string(original, "received_at", "")},
    {"host", safe_string(original, "host", "unknown")},
    {"event_type", safe_string(original, "event_type", "unknown")},
    {"source", safe_string(original, "source", "unknown")},
    {"source_type", safe_string(original, "source_type", "syslog")},
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
  if (requested <= 0) return def;
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
    const AppConfig config = ConfigLoader::load_from_file("config/config.json");

    std::filesystem::create_directories(config.paths.data_dir);
    std::filesystem::create_directories(config.paths.logs_dir);

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
    const std::string rules_path = "config/rules.json";
    std::mutex rules_file_mutex;

    // Rules API
    srv.Get("/api/rules", [&](const httplib::Request&, httplib::Response& res) {
      std::lock_guard<std::mutex> lock(rules_file_mutex);

      json rules;
      std::string err;

      if (!read_json_file(rules_path, rules, err)) {
        send_json_response(res, 500, {
          {"status", "error"},
          {"message", err}
        });
        return;
      }

      if (!rules.is_array()) {
        send_json_response(res, 500, {
          {"status", "error"},
          {"message", "rules file must contain JSON array"}
        });
        return;
      }

      send_json_response(res, 200, {
        {"status", "ok"},
        {"count", rules.size()},
        {"rules", rules}
      });
    });

    srv.Post("/api/rules", [&](const httplib::Request& req, httplib::Response& res) {
      std::lock_guard<std::mutex> lock(rules_file_mutex);

      try {
        json new_rule = json::parse(req.body);

        std::string err;
        if (!validate_rule_json(new_rule, err)) {
          send_json_response(res, 400, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        json rules;
        if (!read_json_file(rules_path, rules, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        if (!rules.is_array()) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", "rules file must contain JSON array"}
          });
          return;
        }

        const std::string id = new_rule["id"].get<std::string>();

        if (find_rule_index_by_id(rules, id) >= 0) {
          send_json_response(res, 409, {
            {"status", "error"},
            {"message", "rule already exists"},
            {"id", id}
          });
          return;
        }

        rules.push_back(new_rule);

        if (!write_json_file(rules_path, rules, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        const bool reloaded = detector.reload_rules();

        send_json_response(res, 201, {
          {"status", "ok"},
          {"action", "created"},
          {"id", id},
          {"rules_reloaded", reloaded},
          {"rule", new_rule}
        });
      } catch (const std::exception& e) {
        send_json_response(res, 400, {
          {"status", "error"},
          {"message", e.what()}
        });
      }
    });

    srv.Put(R"(/api/rules/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
      std::lock_guard<std::mutex> lock(rules_file_mutex);

      try {
        const std::string id = req.matches[1].str();

        json updated_rule = json::parse(req.body);
        updated_rule["id"] = id;

        std::string err;
        if (!validate_rule_json(updated_rule, err)) {
          send_json_response(res, 400, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        json rules;
        if (!read_json_file(rules_path, rules, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        if (!rules.is_array()) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", "rules file must contain JSON array"}
          });
          return;
        }

        const int index = find_rule_index_by_id(rules, id);
        if (index < 0) {
          send_json_response(res, 404, {
            {"status", "error"},
            {"message", "rule not found"},
            {"id", id}
          });
          return;
        }

        rules[static_cast<std::size_t>(index)] = updated_rule;

        if (!write_json_file(rules_path, rules, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        const bool reloaded = detector.reload_rules();

        send_json_response(res, 200, {
          {"status", "ok"},
          {"action", "updated"},
          {"id", id},
          {"rules_reloaded", reloaded},
          {"rule", updated_rule}
        });
      } catch (const std::exception& e) {
        send_json_response(res, 400, {
          {"status", "error"},
          {"message", e.what()}
        });
      }
    });

    srv.Delete(R"(/api/rules/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
      std::lock_guard<std::mutex> lock(rules_file_mutex);

      try {
        const std::string id = req.matches[1].str();

        json rules;
        std::string err;

        if (!read_json_file(rules_path, rules, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        if (!rules.is_array()) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", "rules file must contain JSON array"}
          });
          return;
        }

        const int index = find_rule_index_by_id(rules, id);
        if (index < 0) {
          send_json_response(res, 404, {
            {"status", "error"},
            {"message", "rule not found"},
            {"id", id}
          });
          return;
        }

        json removed_rule = rules[static_cast<std::size_t>(index)];
        rules.erase(rules.begin() + index);

        if (!write_json_file(rules_path, rules, err)) {
          send_json_response(res, 500, {
            {"status", "error"},
            {"message", err}
          });
          return;
        }

        const bool reloaded = detector.reload_rules();

        send_json_response(res, 200, {
          {"status", "ok"},
          {"action", "deleted"},
          {"id", id},
          {"rules_reloaded", reloaded},
          {"rule", removed_rule}
        });
      } catch (const std::exception& e) {
        send_json_response(res, 400, {
          {"status", "error"},
          {"message", e.what()}
        });
      }
    });

    // Ingest endpoint
    srv.Post("/ingest", [&](const httplib::Request& req, httplib::Response& res) {
      try {
        auto j = json::parse(req.body);

        if (!j.is_object()) {
          throw std::runtime_error("ingest body must be a JSON object");
        }

        const std::string received_at = now_iso_utc();

        // Заполняем минимальные поля, если агент не прислал
        std::string ts = safe_string(j, "ts", received_at);
        std::string event_type = safe_string(j, "event_type", "unknown");
        std::string source = safe_string(j, "source", "unknown");
        std::string event_id = safe_string(j, "event_id", "");

	if (!j.contains("ts")) j["ts"] = ts;
	if (!j.contains("event_type")) j["event_type"] = event_type;
	if (!j.contains("source")) j["source"] = source;

        if (event_id.empty()) {
          event_id = generate_event_id();
        }

        j["event_id"] = event_id;
        j["received_at"] = received_at;

        std::string source_type = safe_string(j, "source_type", "");
        if (source_type.empty()) {
          source_type = detect_source_type(j);
        }
        j["source_type"] = source_type;

        ts = safe_string(j, "ts", received_at);
        event_type = safe_string(j, "event_type", "unknown");
        source = safe_string(j, "source", "unknown");

        // Канонизируем json (храним строкой)
        std::string body = j.dump();

        db.insert_event(event_id, ts, received_at, event_type, source, source_type, body);

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

        json out = {
          {"status", "ok"},
          {"event_id", event_id},
          {"received_at", received_at},
          {"source_type", source_type}
        };

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
          {"event_id", r.event_id},
          {"ts", r.ts},
	  {"received_at", r.received_at},
	  {"host", r.host},
          {"event_type", r.event_type},
          {"source", r.source},
          {"source_type", r.source_type},
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
