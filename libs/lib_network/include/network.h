#pragma once

#include <memory>

namespace network {

typedef int descriptor;
static const descriptor kInvalidDesciptor{-1};

class Server {
public:
  Server();
  ~Server() = default;

  void start(const std::string &ip, const uint32_t port);
  void stop();

private:
  class Implementation;
  std::shared_ptr<Implementation> impl_;
};

class Client {
public:
  Client();
  ~Client() = default;

  void start(const std::string &server_ip, const uint32_t server_port);
  void stop();

private:
  class Implementation;
  std::shared_ptr<Implementation> impl_;
};

} // namespace network
