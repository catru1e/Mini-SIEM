#include "db_sqlite.hpp"
#include "../../third_party/json.hpp"

#include <sqlite3.h>

#include <iostream>
#include <chrono>
#include <ctime>
#include <stdexcept>
#include <sstream>

using json = nlohmann::json;

static void throw_sqlite(int rc, sqlite3* db, const char* where) {
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
    std::ostringstream oss;
    oss << where << " failed: rc=" << rc;
    if (db) oss << " err=" << sqlite3_errmsg(db);
    throw std::runtime_error(oss.str());
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

  const char* sql = R"SQL(
    PRAGMA journal_mode=WAL;

    CREATE TABLE IF NOT EXISTS events (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      ts TEXT NOT NULL,
      received_at TEXT NOT NULL,
      host TEXT,
      event_type TEXT NOT NULL,
      source TEXT NOT NULL,
      severity TEXT,
      json TEXT NOT NULL
    );

    CREATE INDEX IF NOT EXISTS idx_events_ts ON events(ts);
    CREATE INDEX IF NOT EXISTS idx_events_type ON events(event_type);
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

  sqlite3_close(db);
}

void SqliteDb::insert_event(const std::string& ts,
                            const std::string& event_type,
                            const std::string& source,
                            const std::string& json_str) {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  std::string received_at = now_iso_utc();
  std::string host = "unknown";
  std::string severity = "info";

  try {
    auto j = json::parse(json_str);

    if (j.contains("host") && j["host"].is_string()) {
      host = j["host"].get<std::string>();
    }
    if (j.contains("severity") && j["severity"].is_string()){
      severity = j["severity"].get<std::string>();
    }
  } catch (...) {
    //keep defaults if JSON parsing fails
  }

  const char* sql = "INSERT INTO events(ts,received_at,host,event_type,source,severity,json) VALUES(?,?,?,?,?,?,?);";
  sqlite3_stmt* stmt = nullptr;

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, ts.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, received_at.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, host.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 4, event_type.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 5, source.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 6, severity.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 7, json_str.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  throw_sqlite(rc, db, "sqlite3_step(insert)");

  sqlite3_finalize(stmt);
  sqlite3_close(db);
}

std::vector<DbEventRow> SqliteDb::get_last_events(int limit) {
  if (limit <= 0) limit = 100;
  if (limit > 1000) limit = 1000;

  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  const char* sql =
	"SELECT id, ts, received_at, host, event_type, source, severity, json FROM events ORDER BY id DESC LIMIT ?;";
  sqlite3_stmt* stmt = nullptr;

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_int(stmt, 1, limit);

  std::vector<DbEventRow> rows;
  while (true) {
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
      DbEventRow r;
      r.id = sqlite3_column_int64(stmt, 0);
      r.ts = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
      r.received_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
      r.host = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
      r.event_type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
      r.source = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
      r.severity = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
      r.json = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
      rows.push_back(std::move(r));
      continue;
    }
    if (rc == SQLITE_DONE) break;
    throw_sqlite(rc, db, "sqlite3_step(select)");
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return rows;
}


// !!!!!!!!!!!!!!!!! ALERT PART !!!!!!!!!!!!!!!!!!!
long long SqliteDb::count_auth_failed_by_src_ip_since(const std::string& src_ip, const std::string& since_ts) {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

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

  const char* sql =
    "SELECT COUNT(*) "
    "FROM events "
    "WHERE event_type = 'auth_failed' "
    "AND ts >= ? "
    "AND json_extract(json, '$.user') = ?;";

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqilte3_prepare_v2");

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
