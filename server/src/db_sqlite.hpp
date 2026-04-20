#pragma once

#include <string>
#include <vector>

struct DbEventRow {
  long long id = 0;
  std::string ts;
  std::string received_at;
  std::string host;
  std::string event_type;
  std::string source;
  std::string severity;
  std::string json;
};

struct DbAlertRow {
  long long id = 0;
  std::string ts;
  std::string rule_name;
  std::string severity;
  std::string title;
  std::string description;
  std::string json;
};

class SqliteDb {
public:
  explicit SqliteDb(std::string db_path);

  // delaem tablicu esli netu
  void init();

  // zapishem shtuchki
  void insert_event(const std::string& ts,
                    const std::string& event_type,
                    const std::string& source,
                    const std::string& json);

  //prochitat poslednie N shtuki (id DESC)
  std::vector<DbEventRow> get_last_events(int limit);
  std::vector<DbAlertRow> get_last_alerts(int limit); //ATTENTION IT IS ALERTS 
  std::vector<std::string> get_recent_privilege_escalation_commands_by_user_since(
    const std::string& user,
    const std::string& since_ts,
    int limit);

  long long count_auth_failed_by_src_ip_since(const std::string& src_ip, const std::string& since_ts);
  long long count_auth_failed_by_user_since(const std::string& user, const std::string& since_ts);
  long long count_auth_invalid_user_by_src_ip_since(const std::string& src_ip, const std::string& since_ts);
  long long count_privilege_escalation_by_user_since(const std::string& user, const std::string& since_ts);
  long long count_auth_success_by_user_since(const std::string& user, const std::string& since_ts);
  bool has_recent_alert_for_rule_and_user_since(
    const std::string& rule_name,
    const std::string& user,
    const std::string& since_ts);

  void insert_alert(const std::string& ts,
                    const std::string& rule_name,
                    const std::string& severity,
                    const std::string& title,
                    const std::string& description,
                    const std::string& json);
private:
  std::string db_path_;
};
