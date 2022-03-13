#pragma once

#include <memory>

namespace network {

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

} // namespace network
