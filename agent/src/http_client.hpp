#pragma once

#include <string>

class HttpClient {
public:
  HttpClient(std::string host, int port);

  void set_bearer_token(const std::string& token);

  bool post_json(const std::string& path,
                 const std::string& body,
                 std::string& response_text);

private:
  std::string host_;
  int port_ = 0;
  std::string bearer_token_;
};
