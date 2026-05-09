#pragma once

#include <string>

struct AuthSession {
  std::string id;
  std::string username;
  std::string role;
  std::string created_at;
  std::string expires_at;
};

class SessionStore {
public:
  SessionStore(std::string db_path, int ttl_seconds);

  void init();

  std::string create_session(const std::string& username,
                             const std::string& role);

  bool find_session(const std::string& id, AuthSession& session);

  void delete_session(const std::string& id);

  void delete_expired();

private:
  std::string db_path_;
  int ttl_seconds_ = 86400;

  std::string make_session_id();
};
