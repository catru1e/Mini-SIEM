#include "http_client.hpp"
#include "httplib.h"

HttpClient::HttpClient(std::string host, int port)
  : host_(std::move(host)), port_(port) {}

bool HttpClient::post_json(const std::string& path, const std::string& body, std::string& response_text) {
  httplib::Client cli(host_, port_);
  cli.set_connection_timeout(3, 0);
  cli.set_read_timeout(5, 0);
  cli.set_write_timeout(5, 0);

  auto res = cli.Post(path.c_str(), body, "application/json");
  if (!res) {
    response_text = "connection_failed";
    return false;
  }

  response_text = res->body;
  return res->status == 200;
}
