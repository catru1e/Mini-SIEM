#pragma once

#include <string>

class HttpClient {
public:
  HttpClient(std::string host, int port);

  // true if HTTP 200
  bool post_json(const std::string& path, const std::string& body, std::string& response_text);

private:
  std::string host_;
  int port_;
};
