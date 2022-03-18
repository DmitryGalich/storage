#include "network.h"

#include <arpa/inet.h>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <fcntl.h>
#include <mutex>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include "logger.h"

namespace network {

typedef int descriptor;
static const descriptor kInvalidDesciptor{-1};

class Server::Implementation {
public:
  Implementation() = default;
  ~Implementation() { stop(); };

  void start(const std::string &ip, const uint32_t port);
  void stop();

private:
  struct Data {
    struct ConnectingData {
      struct sockaddr_in server_address_data_;
      std::unique_ptr<std::thread> thread_;
      std::atomic_bool is_need_cycling_{false};
      std::vector<descriptor> clients_;
      descriptor server_{kInvalidDesciptor};
      descriptor epoll_{kInvalidDesciptor};
      struct epoll_event event_;
    } connecting_data_;

    const int kMaxSocketsRequestsCount_{4096};

  } data_;

  void run_connecting_thread(const std::string &ip, const uint32_t port);
};

void Server::Implementation::start(const std::string &ip, const uint32_t port) {

  log::info("starting...", __PRETTY_FUNCTION__);

  data_.connecting_data_.thread_.reset(
      new std::thread([&]() { run_connecting_thread(ip, port); }));

  log::info("started", __PRETTY_FUNCTION__);
}

void Server::Implementation::stop() {
  log::info("stopping...", __PRETTY_FUNCTION__);

  if (data_.connecting_data_.thread_) {
    if (data_.connecting_data_.is_need_cycling_.load())
      data_.connecting_data_.is_need_cycling_.store(false);

    data_.connecting_data_.thread_->join();
  }

  log::info("stopped", __PRETTY_FUNCTION__);
}

void Server::Implementation::run_connecting_thread(const std::string &ip,
                                                   const uint32_t port) {

  const auto configure = [&]() {
    data_.connecting_data_.server_address_data_.sin_family = AF_INET;
    data_.connecting_data_.server_address_data_.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(),
                  &(data_.connecting_data_.server_address_data_.sin_addr)) != 1)
      throw std::runtime_error("inet_pton() - " + std::string(strerror(errno)));

    data_.connecting_data_.server_ = socket(AF_INET, SOCK_STREAM, 0);
    if (data_.connecting_data_.server_ < 0)
      throw std::runtime_error("socket() - " + std::string(strerror(errno)));

    if (fcntl(data_.connecting_data_.server_, F_SETFL, O_NONBLOCK) < 0)
      throw std::runtime_error("fcntl() - " + std::string(strerror(errno)));

    int opt = 1;
    if (setsockopt(data_.connecting_data_.server_, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
      throw std::runtime_error("setsockopt() - " +
                               std::string(strerror(errno)));

    if (bind(data_.connecting_data_.server_,
             (struct sockaddr *)&data_.connecting_data_.server_address_data_,
             sizeof(data_.connecting_data_.server_address_data_)) < 0)
      throw std::runtime_error("bind() - " + std::string(strerror(errno)));

    if (listen(data_.connecting_data_.server_,
               data_.kMaxSocketsRequestsCount_) < 0)
      throw std::runtime_error("listen() - " + std::string(strerror(errno)));

    data_.connecting_data_.epoll_ = epoll_create(1);
    if (data_.connecting_data_.epoll_ < 0)
      throw std::runtime_error("epoll_create() - " +
                               std::string(strerror(errno)));

    data_.connecting_data_.event_.data.fd = data_.connecting_data_.server_;
    data_.connecting_data_.event_.events = EPOLLIN | EPOLLOUT;
    if (epoll_ctl(data_.connecting_data_.epoll_, EPOLL_CTL_ADD,
                  data_.connecting_data_.server_,
                  &data_.connecting_data_.event_) < 0)
      throw std::runtime_error("epoll_ctl() - " + std::string(strerror(errno)));
  };

  const auto run_cycle = [&]() {
    using namespace std::chrono_literals;
    static const int kWaitingTimeout(0);

    data_.connecting_data_.is_need_cycling_.store(true);

    while (data_.connecting_data_.is_need_cycling_.load()) {
      log::info("epoll_wait() attempt", __PRETTY_FUNCTION__);

      const int kEventsCount = epoll_wait(
          data_.connecting_data_.epoll_, &data_.connecting_data_.event_,
          data_.kMaxSocketsRequestsCount_, kWaitingTimeout);
      if (kEventsCount == -1)
        throw std::runtime_error("epoll_wait() - " +
                                 std::string(strerror(errno)));

      std::this_thread::sleep_for(500ms);
    }

    data_.connecting_data_.is_need_cycling_.store(false);
  };

  const auto make_stop = [&]() {
    for (auto &client : data_.connecting_data_.clients_)
      close(client);

    data_.connecting_data_.clients_.clear();

    if (data_.connecting_data_.server_ >= 0)
      close(data_.connecting_data_.server_);

    data_.connecting_data_.server_ = kInvalidDesciptor;
  };

  // =======================================

  try {
    configure();
    run_cycle();
  } catch (const std::exception &exception) {
    log::error("Connecting thread error", __PRETTY_FUNCTION__);
  }

  make_stop();
}

Server::Server() : impl_(new Implementation) {}

void Server::start(const std::string &ip, const uint32_t port) {
  impl_->start(ip, port);
}

void Server::stop() { impl_->stop(); }

} // namespace network
