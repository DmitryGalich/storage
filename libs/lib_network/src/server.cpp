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
static const int kEpollMaxEvents_{4096};

class Server::Implementation {
public:
  Implementation() = default;
  ~Implementation() { stop(); };

  void start(const std::string &ip, const uint32_t port);
  void stop();

private:
  struct ConnectingData {
    const int kMaxSocketsRequestsCount_{4096};

    std::unique_ptr<std::thread> thread_;
    std::atomic_bool is_need_cycling_{false};

    struct sockaddr_in server_address_data_;
    descriptor server_{kInvalidDesciptor};

    descriptor epoll_{kInvalidDesciptor};
    struct epoll_event events_[kEpollMaxEvents_];
  } connecting_data_;

  void run_connecting_thread(const std::string &ip, const uint32_t port);
};

void Server::Implementation::start(const std::string &ip, const uint32_t port) {

  log::info("starting...", __PRETTY_FUNCTION__);

  connecting_data_.thread_.reset(
      new std::thread([&]() { run_connecting_thread(ip, port); }));

  log::info("started", __PRETTY_FUNCTION__);
}

void Server::Implementation::stop() {
  log::info("stopping...", __PRETTY_FUNCTION__);

  if (connecting_data_.thread_) {
    if (connecting_data_.is_need_cycling_.load())
      connecting_data_.is_need_cycling_.store(false);

    connecting_data_.thread_->join();
  }

  log::info("stopped", __PRETTY_FUNCTION__);
}

void Server::Implementation::run_connecting_thread(const std::string &ip,
                                                   const uint32_t port) {

  const auto configure = [&]() {
    const auto configure_socket = [&]() {
      connecting_data_.server_address_data_.sin_family = AF_INET;
      connecting_data_.server_address_data_.sin_port = htons(port);

      if (inet_pton(AF_INET, ip.c_str(),
                    &(connecting_data_.server_address_data_.sin_addr)) != 1)
        throw std::runtime_error("inet_pton() - " +
                                 std::string(strerror(errno)));

      connecting_data_.server_ = socket(AF_INET, SOCK_STREAM, 0);
      if (connecting_data_.server_ < 0)
        throw std::runtime_error("socket() - " + std::string(strerror(errno)));

      if (fcntl(connecting_data_.server_, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("fcntl() - " + std::string(strerror(errno)));

      int opt = 1;
      if (setsockopt(connecting_data_.server_, SOL_SOCKET,
                     SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
        throw std::runtime_error("setsockopt() - " +
                                 std::string(strerror(errno)));

      if (bind(connecting_data_.server_,
               (struct sockaddr *)&connecting_data_.server_address_data_,
               sizeof(connecting_data_.server_address_data_)) < 0)
        throw std::runtime_error("bind() - " + std::string(strerror(errno)));

      if (listen(connecting_data_.server_,
                 connecting_data_.kMaxSocketsRequestsCount_) < 0)
        throw std::runtime_error("listen() - " + std::string(strerror(errno)));
    };

    const auto configure_epoll = [&]() {
      connecting_data_.epoll_ = epoll_create(1);
      if (connecting_data_.epoll_ < 0)
        throw std::runtime_error("epoll_create() - " +
                                 std::string(strerror(errno)));

      struct epoll_event event;
      event.data.fd = connecting_data_.server_;
      event.events = EPOLLIN | EPOLLOUT;
      if (epoll_ctl(connecting_data_.epoll_, EPOLL_CTL_ADD,
                    connecting_data_.server_, &event) < 0)
        throw std::runtime_error("epoll_ctl() - " +
                                 std::string(strerror(errno)));
    };

    // =======================================

    configure_socket();
    configure_epoll();
  };

  const auto run_cycle = [&]() {
    const auto process_new_connection = [&]() {
      static descriptor client(-1);
      static struct sockaddr_in client_addr;
      socklen_t socklen = sizeof(client_addr);

      log::info("processing new connection...", __PRETTY_FUNCTION__);

      client = accept(connecting_data_.server_, (struct sockaddr *)&client_addr,
                      &socklen);

      if (client < 0)
        throw std::runtime_error("accept() - " + std::string(strerror(errno)));

      if (fcntl(client, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("fcntl() - " + std::string(strerror(errno)));

      struct epoll_event event;
      event.events = EPOLLIN | EPOLLET;
      event.data.fd = client;
      if (epoll_ctl(connecting_data_.epoll_, EPOLL_CTL_ADD, client, &event) < 0)
        throw std::runtime_error("epoll_ctl() - " +
                                 std::string(strerror(errno)));

      log::info("new connection processed", __PRETTY_FUNCTION__);
    };

    using namespace std::chrono_literals;
    static const int kWaitingTimeout(0);

    connecting_data_.is_need_cycling_.store(true);

    while (connecting_data_.is_need_cycling_.load()) {
      log::info("epoll_wait() attempt", __PRETTY_FUNCTION__);

      const int kEventsCount = epoll_wait(
          connecting_data_.epoll_, connecting_data_.events_,
          connecting_data_.kMaxSocketsRequestsCount_, kWaitingTimeout);
      if (kEventsCount == -1)
        throw std::runtime_error("epoll_wait() - " +
                                 std::string(strerror(errno)));

      for (int event_i = 0; event_i < kEventsCount; event_i++) {
        if (connecting_data_.events_[event_i].data.fd ==
            connecting_data_.server_)
          process_new_connection();
      }

      std::this_thread::sleep_for(500ms);
    }

    connecting_data_.is_need_cycling_.store(false);
  };

  const auto make_stop = [&]() {
    if (connecting_data_.server_ >= 0)
      close(connecting_data_.server_);

    connecting_data_.server_ = kInvalidDesciptor;
  };

  // =======================================

  try {
    configure();
    run_cycle();
  } catch (const std::exception &exception) {
    log::error("connecting thread error", __PRETTY_FUNCTION__);
  }

  make_stop();
}

Server::Server() : impl_(new Implementation) {}

void Server::start(const std::string &ip, const uint32_t port) {
  impl_->start(ip, port);
}

void Server::stop() { impl_->stop(); }

} // namespace network
