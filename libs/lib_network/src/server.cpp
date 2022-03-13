#include "network.h"

#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <mutex>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include "logger.h"

namespace network {

class Server::Implementation {
public:
  Implementation() = default;
  ~Implementation() { stop(); };

  void start(const std::string &ip, const uint32_t port);
  void stop();

private:
  typedef int socket_descriptor;

  const int kMaxSocketsRequestsCount_{4096};
  const int kInvalidSocketDesciptor{-1};

  void run_connecting_thread(const std::string &ip, const uint32_t port);

  std::vector<socket_descriptor> clients_;
  socket_descriptor server_{kInvalidSocketDesciptor};
  struct sockaddr_in server_address_data_;

  std::unique_ptr<std::thread> connecting_thread_;
  std::unique_ptr<std::thread> epoll_thread_;
};

void Server::Implementation::start(const std::string &ip, const uint32_t port) {

  log::info("starting...", __PRETTY_FUNCTION__);

  connecting_thread_.reset(
      new std::thread([&]() { run_connecting_thread(ip, port); }));

  log::info("started", __PRETTY_FUNCTION__);
}

void Server::Implementation::stop() {
  log::info("stopping...", __PRETTY_FUNCTION__);

  if (connecting_thread_)
    connecting_thread_->join();

  log::info("stopped", __PRETTY_FUNCTION__);
}

void Server::Implementation::run_connecting_thread(const std::string &ip,
                                                   const uint32_t port) {

  const auto configure = [&]() {
    server_address_data_.sin_family = AF_INET;
    server_address_data_.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &(server_address_data_.sin_addr)) != 1)
      throw std::runtime_error("inet_pton() - " + std::string(strerror(errno)));

    server_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_ < 0)
      throw std::runtime_error("socket() - " + std::string(strerror(errno)));

    //      if (fcntl(server_, F_SETFL, O_NONBLOCK) < 0)
    //        handle_error("fcntl() - " + std::string(strerror(errno)));

    int opt = 1;
    if (setsockopt(server_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)) < 0)
      throw std::runtime_error("setsockopt() - " +
                               std::string(strerror(errno)));

    if (bind(server_, (struct sockaddr *)&server_address_data_,
             sizeof(server_address_data_)) < 0)
      throw std::runtime_error("bind() - " + std::string(strerror(errno)));
  };

  const auto run_connecting_cycle = [&]() {
    while (true) {
      if (listen(server_, kMaxSocketsRequestsCount_) < 0)
        throw std::runtime_error("listen() - " + std::string(strerror(errno)));
    }
  };

  const auto make_stop = [&]() {
    for (auto &client : clients_)
      close(client);

    clients_.clear();

    if (server_ >= 0)
      close(server_);

    server_ = kInvalidSocketDesciptor;
  };

  // =======================================

  try {
    configure();
    run_connecting_cycle();
  } catch (const std::exception &exception) {
  }
  make_stop();
}

Server::Server() : impl_(new Implementation) {}

void Server::start(const std::string &ip, const uint32_t port) {
  impl_->start(ip, port);
}

void Server::stop() { impl_->stop(); }

} // namespace network
