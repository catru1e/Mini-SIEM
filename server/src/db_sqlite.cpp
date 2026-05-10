#include "db_sqlite.hpp"
#include "../../third_party/json.hpp"

#include <sqlite3.h>

#include <iostream>
#include <chrono>
#include <ctime>
#include <stdexcept>
#include <sstream>

#include <algorithm>
#include <map>

using json = nlohmann::json;

static void throw_sqlite(int rc, sqlite3* db, const char* where) {
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
    std::ostringstream oss;
    oss << where << " failed: rc=" << rc;
    if (db) oss << " err=" << sqlite3_errmsg(db);
    throw std::runtime_error(oss.str());
  }
}

static std::string sqlite_text(sqlite3_stmt* stmt, int col) {
  const unsigned char* value = sqlite3_column_text(stmt, col);
  if (!value) return "";
  return reinterpret_cast<const char*>(value);
}

static bool column_exists(sqlite3* db, const std::string& table, const std::string& column) {
  std::string sql = "PRAGMA table_info(" + table + ");";

  sqlite3_stmt* stmt = nullptr;
  int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK) {
    return false;
  }

  bool found = false;

  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    std::string name = sqlite_text(stmt, 1);
    if (name == column) {
      found = true;
      break;
    }
  }

  sqlite3_finalize(stmt);
  return found;
}

static void add_text_column_if_missing(sqlite3* db, const std::string& table, const std::string& column) {
  if (column_exists(db, table, column)) {
    return;
  }

  std::string sql = "ALTER TABLE " + table + " ADD COLUMN " + column + " TEXT;";

  char* errmsg = nullptr;
  int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errmsg);
  if (rc != SQLITE_OK) {
    std::string err = errmsg ? errmsg : "unknown";
    sqlite3_free(errmsg);
    throw std::runtime_error("sqlite3_exec alter failed: " + err);
  }
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

SqliteDb::SqliteDb(std::string db_path)
  : db_path_(std::move(db_path)) {}

void SqliteDb::init() {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  if (rc != SQLITE_OK) {
    std::string err = db ? sqlite3_errmsg(db) : "unknown";
    if (db) sqlite3_close(db);
    throw std::runtime_error("sqlite3_open failed: " + err);
  }

  sqlite3_busy_timeout(db, 5000);

  const char* sql = R"SQL(
    PRAGMA journal_mode=WAL;
    PRAGMA synchronous=NORMAL;

    CREATE TABLE IF NOT EXISTS events (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      event_id TEXT,
      ts TEXT NOT NULL,
      received_at TEXT NOT NULL,
      host TEXT,
      event_type TEXT NOT NULL,
      source TEXT NOT NULL,
      source_type TEXT,
      severity TEXT,
      json TEXT NOT NULL
    );

    CREATE INDEX IF NOT EXISTS idx_events_event_id ON events(event_id);
    CREATE INDEX IF NOT EXISTS idx_events_ts ON events(ts);
    CREATE INDEX IF NOT EXISTS idx_events_type ON events(event_type);
    CREATE INDEX IF NOT EXISTS idx_events_source ON events(source);
    CREATE INDEX IF NOT EXISTS idx_events_source_type ON events(source_type);
    CREATE INDEX IF NOT EXISTS idx_events_host ON events(host);
    CREATE INDEX IF NOT EXISTS idx_events_severity ON events(severity);

    CREATE TABLE IF NOT EXISTS alerts (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      ts TEXT NOT NULL,
      rule_name TEXT NOT NULL,
      severity TEXT NOT NULL,
      title TEXT NOT NULL,
      description TEXT NOT NULL,
      json TEXT NOT NULL
    );

    CREATE INDEX IF NOT EXISTS idx_alerts_ts ON alerts(ts);
    CREATE INDEX IF NOT EXISTS idx_alerts_rule_name ON alerts(rule_name);
  )SQL";

  char* errmsg = nullptr;
  rc = sqlite3_exec(db, sql, nullptr, nullptr, &errmsg);
  if (rc != SQLITE_OK) {
    std::string err = errmsg ? errmsg : "unknown";
    sqlite3_free(errmsg);
    sqlite3_close(db);
    throw std::runtime_error("sqlite3_exec init failed: " + err);
  }

  add_text_column_if_missing(db, "events", "event_id");
  add_text_column_if_missing(db, "events", "source_type");

  add_text_column_if_missing(db, "events", "receiver_id");
  add_text_column_if_missing(db, "events", "parser_status");
  add_text_column_if_missing(db, "events", "parser_rule_id");
  add_text_column_if_missing(db, "events", "policy_group_id");
  add_text_column_if_missing(db, "events", "event_name");
  add_text_column_if_missing(db, "events", "fields");

  sqlite3_close(db);
}

void SqliteDb::insert_event(const std::string& event_id,
                            const std::string& ts,
                            const std::string& received_at,
                            const std::string& event_type,
                            const std::string& source,
                            const std::string& source_type,
                            const std::string& json_str) {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  std::string host = "unknown";
  std::string severity = "info";

  std::string receiver_id;
  std::string parser_status;
  std::string parser_rule_id;
  std::string policy_group_id;
  std::string event_name;
  std::string fields_json = "{}";

  try {
    auto j = json::parse(json_str);

    if (j.contains("host") && j["host"].is_string()) {
      host = j["host"].get<std::string>();
    }
    if (j.contains("severity") && j["severity"].is_string()){
      severity = j["severity"].get<std::string>();
    }

    if (j.contains("receiver_id") && j["receiver_id"].is_string()) {
      receiver_id = j["receiver_id"].get<std::string>();
    }

    if (j.contains("parser_status") && j["parser_status"].is_string()) {
      parser_status = j["parser_status"].get<std::string>();
    }

    if (j.contains("parser_rule_id") && j["parser_rule_id"].is_string()) {
      parser_rule_id = j["parser_rule_id"].get<std::string>();
    }

    if (j.contains("policy_group_id") && j["policy_group_id"].is_string()) {
      policy_group_id = j["policy_group_id"].get<std::string>();
    }

    if (j.contains("event_name") && j["event_name"].is_string()) {
      event_name = j["event_name"].get<std::string>();
    }

    if (j.contains("fields") && j["fields"].is_object()) {
      fields_json = j["fields"].dump();
    }

  } catch (...) {
    //keep defaults if JSON parsing fails
  }

  const char* sql =
    "INSERT INTO events("
    "event_id,ts,received_at,host,event_type,source,source_type,severity,"
    "receiver_id,parser_status,parser_rule_id,policy_group_id,event_name,fields,json"
    ") VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);";

  sqlite3_stmt* stmt = nullptr;

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, event_id.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, ts.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, received_at.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 4, host.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 5, event_type.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 6, source.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 7, source_type.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 8, severity.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 9, receiver_id.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 10, parser_status.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 11, parser_rule_id.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 12, policy_group_id.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 13, event_name.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 14, fields_json.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 15, json_str.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  throw_sqlite(rc, db, "sqlite3_step(insert)");

  sqlite3_finalize(stmt);
  sqlite3_close(db);
}

std::vector<DbEventRow> SqliteDb::get_last_events(int limit) {
  if (limit <= 0) limit = 10000;

  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  if (rc != SQLITE_OK) {
    std::cerr << "[db][ERR] get_last_events sqlite3_open failed: "
              << (db ? sqlite3_errmsg(db) : "unknown") << "\n";
    if (db) sqlite3_close(db);
    return {};
  }

  sqlite3_busy_timeout(db, 5000);

  // ВАЖНО: читаем только базовые колонки, которые точно есть.
  // receiver/parser fields берем из json в server/main.cpp.
  const char* sql =
    "SELECT id, event_id, ts, received_at, host, event_type, source, source_type, severity, json "
    "FROM events "
    "ORDER BY id DESC "
    "LIMIT ?;";

  sqlite3_stmt* stmt = nullptr;

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  if (rc != SQLITE_OK) {
    std::cerr << "[db][ERR] get_last_events prepare failed: "
              << sqlite3_errmsg(db) << "\n";
    sqlite3_close(db);
    return {};
  }

  sqlite3_bind_int(stmt, 1, limit);

  std::vector<DbEventRow> rows;

  while (true) {
    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
      DbEventRow r;
      r.id = sqlite3_column_int64(stmt, 0);
      r.event_id = sqlite_text(stmt, 1);
      r.ts = sqlite_text(stmt, 2);
      r.received_at = sqlite_text(stmt, 3);
      r.host = sqlite_text(stmt, 4);
      r.event_type = sqlite_text(stmt, 5);
      r.source = sqlite_text(stmt, 6);
      r.source_type = sqlite_text(stmt, 7);
      r.severity = sqlite_text(stmt, 8);
      r.json = sqlite_text(stmt, 9);

      rows.push_back(std::move(r));
      continue;
    }

    if (rc == SQLITE_DONE) {
      break;
    }

    std::cerr << "[db][ERR] get_last_events step failed: "
              << sqlite3_errmsg(db) << "\n";
    break;
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);

  std::cerr << "[db] get_last_events rows=" << rows.size()
            << " limit=" << limit << "\n";

  return rows;
}

// !!!!!!!!!!!!!!!!! ALERT PART !!!!!!!!!!!!!!!!!!!
long long SqliteDb::count_auth_failed_by_src_ip_since(const std::string& src_ip, const std::string& since_ts) {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
    "SELECT COUNT(*) "
    "FROM events "
    "WHERE event_type = 'auth_failed' "
    "AND ts >= ? "
    "AND json_extract(json, '$.src_ip') = ?;";

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, since_ts.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, src_ip.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    throw_sqlite(rc, db, "sqlite3_step(count_auth_failed_by_src_ip_since)");
  }

  long long count = sqlite3_column_int64(stmt, 0);

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return count;
}

long long SqliteDb::count_auth_failed_by_user_since(const std::string& user, const std::string& since_ts) {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
    "SELECT COUNT(*) "
    "FROM events "
    "WHERE event_type = 'auth_failed' "
    "AND ts >= ? "
    "AND json_extract(json, '$.user') = ?;";

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, since_ts.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, user.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    throw_sqlite(rc, db, "sqlite3_step(count_auth_failed_by_user_since)");
  }

  long long count = sqlite3_column_int64(stmt, 0);

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return count;
}

long long SqliteDb::count_auth_invalid_user_by_src_ip_since(const std::string& src_ip, const std::string& since_ts) {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
    "SELECT COUNT(*) "
    "FROM events "
    "WHERE event_type = 'auth_invalid_user' "
    "AND ts >= ? "
    "AND json_extract(json, '$.src_ip') = ?;";

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, since_ts.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, src_ip.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    throw_sqlite(rc, db, "sqlite3_step(count_auth_invalid_user_by_src_ip_since)");
  }

  long long count = sqlite3_column_int64(stmt, 0);

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return count;
}

long long SqliteDb::count_privilege_escalation_by_user_since(const std::string& user, const std::string& since_ts) {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
    "SELECT COUNT(*) "
    "FROM events "
    "WHERE event_type = 'privilege_escalation' "
    "AND ts >= ? "
    "AND json_extract(json, '$.user') = ?;";

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, since_ts.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, user.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    throw_sqlite(rc, db, "sqlite3_step(count_privilege_escalation_by_user_since)");
  }

  long long count = sqlite3_column_int64(stmt, 0);

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return count;
}

long long SqliteDb::count_auth_success_by_user_since(const std::string& user, const std::string& since_ts) {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
    "SELECT COUNT(*) "
    "FROM events "
    "WHERE event_type = 'auth_success' "
    "AND ts >= ? "
    "AND json_extract(json, '$.user') = ?;";

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, since_ts.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, user.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    throw_sqlite(rc, db, "sqlite3_step(count_auth_success_by_user_since)");
  }

  long long count = sqlite3_column_int64(stmt, 0);

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return count;
}

bool SqliteDb::has_recent_alert_for_rule_and_user_since(
    const std::string& rule_name,
    const std::string& user,
    const std::string& since_ts) {

  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
    "SELECT COUNT(*) "
    "FROM alerts "
    "WHERE rule_name = ? "
    "AND ts >= ? "
    "AND json_extract(json, '$.user') = ?;";

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, rule_name.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, since_ts.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, user.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    throw_sqlite(rc, db, "sqlite3_step(has_recent_alert_for_rule_and_user_since)");
  }

  const long long count = sqlite3_column_int64(stmt, 0);

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return count > 0;
}

void SqliteDb::insert_alert(const std::string& ts,
			    const std::string& rule_name,
			    const std::string& severity,
			    const std::string& title,
			    const std::string& description,
			    const std::string& json_str) {
  std::cerr << "[db] insert_alert rule_name=" << rule_name
          << " severity=" << severity
          << " title=" << title << "\n";

  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql = "INSERT INTO alerts(ts, rule_name, severity, title, description, json) "
		    "VALUES(?,?,?,?,?,?);";
  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, ts.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, rule_name.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, severity.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 4, title.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 5, description.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 6, json_str.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  throw_sqlite(rc, db, "sqlite3_step(insert_alert)");

  sqlite3_finalize(stmt);
  sqlite3_close(db);
}


std::vector<DbAlertRow> SqliteDb::get_last_alerts(int limit) {
  if (limit <= 0) limit = 10000;

  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
      "SELECT id, ts, rule_name, severity, title, description, json "
      "FROM alerts "
      "ORDER BY id DESC "
      "LIMIT ?;";

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_int(stmt, 1, limit);

  std::vector<DbAlertRow> rows;
  while (true) {
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
      DbAlertRow r;
      r.id = sqlite3_column_int64(stmt, 0);
      r.ts = sqlite_text(stmt, 1);
      r.rule_name = sqlite_text(stmt, 2);
      r.severity = sqlite_text(stmt, 3);
      r.title = sqlite_text(stmt, 4);
      r.description = sqlite_text(stmt, 5);
      r.json = sqlite_text(stmt, 6);
      rows.push_back(std::move(r));
      continue;
    }
    if (rc == SQLITE_DONE) break;
    throw_sqlite(rc, db, "sqlite3_step(get_last_alerts)");
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return rows;
}

std::vector<std::string> SqliteDb::get_recent_privilege_escalation_commands_by_user_since(
    const std::string& user,
    const std::string& since_ts,
    int limit) {

  std::vector<std::string> commands;

  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  if (rc != SQLITE_OK) {
    return commands;
  }

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
      "SELECT json FROM events "
      "WHERE event_type = 'privilege_escalation' AND ts >= ? "
      "ORDER BY ts DESC;";

  sqlite3_stmt* stmt = nullptr;

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    return commands;
  }

  sqlite3_bind_text(stmt, 1, since_ts.c_str(), -1, SQLITE_TRANSIENT);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const unsigned char* json_text = sqlite3_column_text(stmt, 0);
    if (!json_text) continue;

    try {
      auto j = nlohmann::json::parse(reinterpret_cast<const char*>(json_text));

      if (!j.contains("user") || !j["user"].is_string()) continue;
      if (j["user"].get<std::string>() != user) continue;

      if (!j.contains("command") || !j["command"].is_string()) continue;

      const std::string cmd = j["command"].get<std::string>();

      // убрать дубликаты
      bool exists = false;
      for (const auto& existing : commands) {
        if (existing == cmd) {
          exists = true;
          break;
        }
      }

      if (!exists) {
        commands.push_back(cmd);
      }

      if (static_cast<int>(commands.size()) >= limit) break;

    } catch (...) {
      continue;
    }
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return commands;
}

long long SqliteDb::count_events_by_field_since(const std::string& event_type,
                                                const std::string& field_name,
                                                const std::string& field_value,
                                                const std::string& since_ts) {
  if (event_type.empty()) return 0;
  if (field_name.empty()) return 0;
  if (field_value.empty()) return 0;

  for (char c : field_name) {
    const bool ok =
      (c >= 'a' && c <= 'z') ||
      (c >= 'A' && c <= 'Z') ||
      (c >= '0' && c <= '9') ||
      c == '_' ||
      c == '.' ||
      c == '-';

    if (!ok) {
      return 0;
    }
  }

  std::string alias = field_name;

  if (field_name == "source.ip") alias = "src_ip";
  else if (field_name == "client.ip") alias = "src_ip";
  else if (field_name == "observer.ip") alias = "src_ip";

  else if (field_name == "source.port") alias = "src_port";
  else if (field_name == "client.port") alias = "src_port";

  else if (field_name == "destination.ip") alias = "dst_ip";
  else if (field_name == "server.ip") alias = "dst_ip";

  else if (field_name == "destination.port") alias = "dst_port";
  else if (field_name == "server.port") alias = "dst_port";

  else if (field_name == "user.name") alias = "user";
  else if (field_name == "source.user.name") alias = "user";
  else if (field_name == "destination.user.name") alias = "user";
  else if (field_name == "account.name") alias = "user";
  else if (field_name == "actor.name") alias = "user";

  else if (field_name == "process.name") alias = "process_name";
  else if (field_name == "process.executable") alias = "process_name";
  else if (field_name == "syslog.identifier") alias = "program";

  else if (field_name == "process.pid") alias = "pid";
  else if (field_name == "syslog.pid") alias = "pid";

  else if (field_name == "process.command_line") alias = "command";
  else if (field_name == "auth.sudo.command") alias = "command";

  else if (field_name == "file.path") alias = "path";
  else if (field_name == "file.target_path") alias = "path";
  else if (field_name == "registry.path") alias = "path";

  else if (field_name == "network.protocol") alias = "protocol";
  else if (field_name == "auth.protocol") alias = "protocol";

  else if (field_name == "event.name") alias = "event_name";
  else if (field_name == "event.code") alias = "event_code";
  else if (field_name == "event.type") alias = "event_type";
  else if (field_name == "event.category") alias = "event_category";
  else if (field_name == "event.action") alias = "event_action";
  else if (field_name == "event.outcome") alias = "event_outcome";
  else if (field_name == "event.severity") alias = "severity";

  else if (field_name == "host.name") alias = "host";
  else if (field_name == "host.hostname") alias = "host";

  else if (field_name == "service.name") alias = "service";
  else if (field_name == "pam.service") alias = "service";

  else if (field_name == "linux.kernel.error_code") alias = "error_code";
  else if (field_name == "event.error_code") alias = "error_code";

  else if (field_name == "url.original") alias = "url";
  else if (field_name == "dns.question.name") alias = "domain";

  else if (field_name == "threat.name") alias = "threat_name";
  else if (field_name == "malware.name") alias = "threat_name";

  const std::string direct_path = "$.\"" + field_name + "\"";
  const std::string fields_path = "$.fields.\"" + field_name + "\"";
  const std::string alias_path = "$.\"" + alias + "\"";

  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
    "SELECT COUNT(*) "
    "FROM events "
    "WHERE event_type = ? "
    "AND ts >= ? "
    "AND ("
      "json_extract(json, ?) = ? "
      "OR json_extract(json, ?) = ? "
      "OR json_extract(json, ?) = ?"
    ");";

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, event_type.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, since_ts.c_str(), -1, SQLITE_TRANSIENT);

  sqlite3_bind_text(stmt, 3, direct_path.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 4, field_value.c_str(), -1, SQLITE_TRANSIENT);

  sqlite3_bind_text(stmt, 5, fields_path.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 6, field_value.c_str(), -1, SQLITE_TRANSIENT);

  sqlite3_bind_text(stmt, 7, alias_path.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 8, field_value.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    throw_sqlite(rc, db, "sqlite3_step(count_events_by_field_since)");
  }

  long long count = sqlite3_column_int64(stmt, 0);

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return count;
}

bool SqliteDb::has_recent_alert_for_rule_and_group_since(const std::string& rule_id,
                                                         const std::string& group_key,
                                                         const std::string& since_ts) {
  if (rule_id.empty()) return false;
  if (group_key.empty()) return false;

  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
    "SELECT COUNT(*) "
    "FROM alerts "
    "WHERE ts >= ? "
    "AND json_extract(json, '$.rule_id') = ? "
    "AND json_extract(json, '$.group_key') = ?;";

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, since_ts.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, rule_id.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, group_key.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    throw_sqlite(rc, db, "sqlite3_step(has_recent_alert_for_rule_and_group_since)");
  }

  const long long count = sqlite3_column_int64(stmt, 0);

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return count > 0;
}


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 20. Dashboard shi !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
static long long scalar_count_query(const std::string& db_path, const char* sql) {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    throw_sqlite(rc, db, "sqlite3_step(scalar_count_query)");
  }

  const long long value = sqlite3_column_int64(stmt, 0);

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return value;
}

static std::map<std::string, long long> group_count_query(
    const std::string& db_path,
    const char* sql) {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  std::map<std::string, long long> result;

  while (true) {
    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
      std::string key = sqlite_text(stmt, 0);
      if (key.empty()) key = "unknown";

      const long long count = sqlite3_column_int64(stmt, 1);
      result[key] = count;
      continue;
    }

    if (rc == SQLITE_DONE) break;

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    throw_sqlite(rc, db, "sqlite3_step(group_count_query)");
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return result;
}

long long SqliteDb::count_all_events() {
  return scalar_count_query(db_path_, "SELECT COUNT(*) FROM events;");
}

long long SqliteDb::count_all_alerts() {
  return scalar_count_query(db_path_, "SELECT COUNT(*) FROM alerts;");
}

std::map<std::string, long long> SqliteDb::count_events_by_event_type() {
  return group_count_query(
    db_path_,
    "SELECT COALESCE(NULLIF(event_type, ''), 'unknown') AS k, COUNT(*) "
    "FROM events "
    "GROUP BY k "
    "ORDER BY COUNT(*) DESC;"
  );
}

std::map<std::string, long long> SqliteDb::count_events_by_source_type() {
  return group_count_query(
    db_path_,
    "SELECT COALESCE(NULLIF(source_type, ''), 'unknown') AS k, COUNT(*) "
    "FROM events "
    "GROUP BY k "
    "ORDER BY COUNT(*) DESC;"
  );
}

std::map<std::string, long long> SqliteDb::count_alerts_by_severity() {
  return group_count_query(
    db_path_,
    "SELECT COALESCE(NULLIF(severity, ''), 'unknown') AS k, COUNT(*) "
    "FROM alerts "
    "GROUP BY k "
    "ORDER BY COUNT(*) DESC;"
  );
}

std::vector<DbTimeBucket> SqliteDb::count_events_over_time(int limit) {
  if (limit <= 0) limit = 60;
  if (limit > 240) limit = 240;

  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
    "SELECT substr(received_at, 1, 16) AS bucket, COUNT(*) "
    "FROM events "
    "WHERE received_at IS NOT NULL AND received_at != '' "
    "GROUP BY bucket "
    "ORDER BY bucket DESC "
    "LIMIT ?;";

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_int(stmt, 1, limit);

  std::vector<DbTimeBucket> rows;

  while (true) {
    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
      DbTimeBucket row;
      row.bucket = sqlite_text(stmt, 0);
      row.count = sqlite3_column_int64(stmt, 1);
      rows.push_back(std::move(row));
      continue;
    }

    if (rc == SQLITE_DONE) break;

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    throw_sqlite(rc, db, "sqlite3_step(count_events_over_time)");
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);

  std::reverse(rows.begin(), rows.end());
  return rows;
}
