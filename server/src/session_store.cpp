#include "session_store.hpp"

#include <sqlite3.h>

#include <chrono>
#include <ctime>
#include <filesystem>
#include <random>
#include <sstream>
#include <stdexcept>
#include <utility>

static void throw_session_sqlite(int rc, sqlite3* db, const char* where) {
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
    std::ostringstream oss;
    oss << where << " failed: rc=" << rc;
    if (db) oss << " err=" << sqlite3_errmsg(db);
    throw std::runtime_error(oss.str());
  }
}

static std::string sqlite_text_session(sqlite3_stmt* stmt, int col) {
  const unsigned char* value = sqlite3_column_text(stmt, col);
  if (!value) return "";
  return reinterpret_cast<const char*>(value);
}

static std::string time_iso_utc_session(std::chrono::system_clock::time_point t) {
  using namespace std::chrono;

  auto secs = time_point_cast<std::chrono::seconds>(t);
  auto ms = duration_cast<milliseconds>(t - secs).count();

  std::time_t tt = system_clock::to_time_t(t);
  std::tm tm{};
  gmtime_r(&tt, &tm);

  char buf[64];
  std::snprintf(buf, sizeof(buf),
                "%04d-%02d-%02dT%02d:%02d:%02d.%03lldZ",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                tm.tm_hour, tm.tm_min, tm.tm_sec,
                static_cast<long long>(ms));

  return std::string(buf);
}

static std::string now_iso_utc_session() {
  return time_iso_utc_session(std::chrono::system_clock::now());
}

SessionStore::SessionStore(std::string db_path, int ttl_seconds)
  : db_path_(std::move(db_path)), ttl_seconds_(ttl_seconds) {
  if (ttl_seconds_ <= 0) {
    ttl_seconds_ = 86400;
  }
}

void SessionStore::init() {
  std::filesystem::path p(db_path_);
  if (p.has_parent_path()) {
    std::filesystem::create_directories(p.parent_path());
  }

  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_session_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql = R"SQL(
    CREATE TABLE IF NOT EXISTS sessions (
      id TEXT PRIMARY KEY,
      username TEXT NOT NULL,
      role TEXT NOT NULL,
      created_at TEXT NOT NULL,
      expires_at TEXT NOT NULL
    );

    CREATE INDEX IF NOT EXISTS idx_sessions_username ON sessions(username);
    CREATE INDEX IF NOT EXISTS idx_sessions_expires_at ON sessions(expires_at);
  )SQL";

  char* errmsg = nullptr;
  rc = sqlite3_exec(db, sql, nullptr, nullptr, &errmsg);
  if (rc != SQLITE_OK) {
    std::string err = errmsg ? errmsg : "unknown";
    sqlite3_free(errmsg);
    sqlite3_close(db);
    throw std::runtime_error("sqlite3_exec sessions init failed: " + err);
  }

  sqlite3_close(db);
}

std::string SessionStore::make_session_id() {
  static const char* hex = "0123456789abcdef";

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(0, 15);

  std::string out;
  out.reserve(64);

  for (int i = 0; i < 64; ++i) {
    out.push_back(hex[dist(gen)]);
  }

  return out;
}

std::string SessionStore::create_session(const std::string& username,
                                         const std::string& role) {
  delete_expired();

  const std::string id = make_session_id();

  auto now = std::chrono::system_clock::now();
  const std::string created_at = time_iso_utc_session(now);
  const std::string expires_at = time_iso_utc_session(now + std::chrono::seconds(ttl_seconds_));

  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_session_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
    "INSERT INTO sessions(id, username, role, created_at, expires_at) "
    "VALUES(?,?,?,?,?);";

  sqlite3_stmt* stmt = nullptr;

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_session_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, role.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 4, created_at.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 5, expires_at.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  throw_session_sqlite(rc, db, "sqlite3_step(create_session)");

  sqlite3_finalize(stmt);
  sqlite3_close(db);

  return id;
}

bool SessionStore::find_session(const std::string& id, AuthSession& session) {
  if (id.empty()) return false;

  delete_expired();

  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_session_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
    "SELECT id, username, role, created_at, expires_at "
    "FROM sessions WHERE id = ? LIMIT 1;";

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_session_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return false;
  }

  AuthSession s;
  s.id = sqlite_text_session(stmt, 0);
  s.username = sqlite_text_session(stmt, 1);
  s.role = sqlite_text_session(stmt, 2);
  s.created_at = sqlite_text_session(stmt, 3);
  s.expires_at = sqlite_text_session(stmt, 4);

  sqlite3_finalize(stmt);
  sqlite3_close(db);

  if (s.expires_at <= now_iso_utc_session()) {
    delete_session(s.id);
    return false;
  }

  session = s;
  return true;
}

void SessionStore::delete_session(const std::string& id) {
  if (id.empty()) return;

  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_session_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql = "DELETE FROM sessions WHERE id = ?;";
  sqlite3_stmt* stmt = nullptr;

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_session_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  throw_session_sqlite(rc, db, "sqlite3_step(delete_session)");

  sqlite3_finalize(stmt);
  sqlite3_close(db);
}

void SessionStore::delete_expired() {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_session_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const std::string now = now_iso_utc_session();

  const char* sql = "DELETE FROM sessions WHERE expires_at <= ?;";
  sqlite3_stmt* stmt = nullptr;

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_session_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, now.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  throw_session_sqlite(rc, db, "sqlite3_step(delete_expired)");

  sqlite3_finalize(stmt);
  sqlite3_close(db);
}
