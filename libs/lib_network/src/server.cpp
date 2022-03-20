#include "network.h"

#include <arpa/inet.h>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <fcntl.h>
#include <mutex>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include "logger.h"

namespace network {

static const int kEpollMaxEvents_{4096};

class Server::Implementation {
public:
  Implementation() = default;
  ~Implementation() { stop(); };

  void start(const std::string &ip, const uint32_t port);
  void stop();

private:
  struct CommunicationData {
    const int kMaxSocketsRequestsCount_{4096};

    std::unique_ptr<std::thread> thread_;
    std::atomic_bool is_need_cycling_{false};

    struct sockaddr_in server_address_data_;
    descriptor server_{kInvalidDesciptor};

    descriptor epoll_fd{kInvalidDesciptor};
    struct epoll_event events_[kEpollMaxEvents_];

    std::string ip_;
    uint32_t port_;

    void clear() {
      is_need_cycling_.store(false);

      if (thread_)
        thread_->join();

      thread_.reset();

      ip_.clear();
      port_ = 0;

      epoll_fd = kInvalidDesciptor;
      server_ = kInvalidDesciptor;
    }
  } communication_data_;

  void run_communicating_thread();
  void configure_socket();
  void configure_epoll();
  void run_communication_cycle();
  void stop_server();
  void process_new_connection();
  void process_existing_connection(const descriptor &client_fd);
  void close_connection(const descriptor &client_fd);
};

void Server::Implementation::start(const std::string &ip, const uint32_t port) {
  communication_data_.clear();
  communication_data_.ip_ = ip;
  communication_data_.port_ = port;

  log::info("starting...", __PRETTY_FUNCTION__);

  communication_data_.thread_.reset(
      new std::thread([&]() { run_communicating_thread(); }));

  log::info("started", __PRETTY_FUNCTION__);
}

void Server::Implementation::stop() {
  log::info("stopping...", __PRETTY_FUNCTION__);

  communication_data_.clear();

  log::info("stopped", __PRETTY_FUNCTION__);
}

void Server::Implementation::run_communicating_thread() {

  try {
    configure_socket();
    configure_epoll();
    run_communication_cycle();
  } catch (const std::exception &exception) {
    log::error("communication thread error(" + std::string(exception.what()) +
                   ")",
               __PRETTY_FUNCTION__);
  }

  stop_server();
}

void Server::Implementation::configure_socket() {
  communication_data_.server_address_data_.sin_family = AF_INET;
  communication_data_.server_address_data_.sin_port =
      htons(communication_data_.port_);

  if (inet_pton(AF_INET, communication_data_.ip_.c_str(),
                &(communication_data_.server_address_data_.sin_addr)) != 1)
    throw std::runtime_error("inet_pton() - " + std::string(strerror(errno)));

  communication_data_.server_ = socket(AF_INET, SOCK_STREAM, 0);
  if (communication_data_.server_ < 0)
    throw std::runtime_error("socket() - " + std::string(strerror(errno)));

  if (fcntl(communication_data_.server_, F_SETFL, O_NONBLOCK) < 0)
    throw std::runtime_error("fcntl() - " + std::string(strerror(errno)));

  int opt = 1;
  if (setsockopt(communication_data_.server_, SOL_SOCKET,
                 SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
    throw std::runtime_error("setsockopt() - " + std::string(strerror(errno)));

  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
    throw std::runtime_error("signal() - " + std::string(strerror(errno)));

  if (bind(communication_data_.server_,
           (struct sockaddr *)&communication_data_.server_address_data_,
           sizeof(communication_data_.server_address_data_)) < 0)
    throw std::runtime_error("bind() - " + std::string(strerror(errno)));

  if (listen(communication_data_.server_,
             communication_data_.kMaxSocketsRequestsCount_) < 0)
    throw std::runtime_error("listen() - " + std::string(strerror(errno)));
}

void Server::Implementation::configure_epoll() {
  communication_data_.epoll_fd = epoll_create(1);
  if (communication_data_.epoll_fd < 0)
    throw std::runtime_error("epoll_create() - " +
                             std::string(strerror(errno)));

  struct epoll_event event;
  event.data.fd = communication_data_.server_;
  event.events = EPOLLIN | EPOLLOUT;
  if (epoll_ctl(communication_data_.epoll_fd, EPOLL_CTL_ADD,
                communication_data_.server_, &event) < 0)
    throw std::runtime_error("epoll_ctl() - " + std::string(strerror(errno)));
}

void Server::Implementation::run_communication_cycle() {

  using namespace std::chrono_literals;
  static const int kWaitingTimeout(0);

  communication_data_.is_need_cycling_.store(true);

  while (communication_data_.is_need_cycling_.load()) {
    log::info("epoll_wait() attempt", __PRETTY_FUNCTION__);

    const int kEventsCount = epoll_wait(
        communication_data_.epoll_fd, communication_data_.events_,
        communication_data_.kMaxSocketsRequestsCount_, kWaitingTimeout);
    if (kEventsCount == -1)
      throw std::runtime_error("epoll_wait() - " +
                               std::string(strerror(errno)));

    for (int event_i = 0; event_i < kEventsCount; event_i++) {
      if (communication_data_.events_[event_i].data.fd ==
          communication_data_.server_) {
        process_new_connection();
      } else {
        try {
          process_existing_connection(
              communication_data_.events_[event_i].data.fd);
        } catch (const std::exception &exception) {
          log::error(std::string(exception.what()), __PRETTY_FUNCTION__);
          close_connection(communication_data_.events_[event_i].data.fd);
        }
      }

      if (communication_data_.events_[event_i].events & (EPOLLRDHUP | EPOLLHUP))
        close_connection(communication_data_.events_[event_i].data.fd);
    }

    std::this_thread::sleep_for(500ms);
  }

  communication_data_.is_need_cycling_.store(false);
}

void Server::Implementation::stop_server() {
  if (communication_data_.server_ >= 0)
    close(communication_data_.server_);
}

void Server::Implementation::process_new_connection() {
  static descriptor client(-1);
  static struct sockaddr_in client_addr;
  socklen_t socklen = sizeof(client_addr);

  log::info("processing new connection...", __PRETTY_FUNCTION__);

  client = accept(communication_data_.server_, (struct sockaddr *)&client_addr,
                  &socklen);

  if (client < 0)
    throw std::runtime_error("accept() - " + std::string(strerror(errno)));

  if (fcntl(client, F_SETFL, O_NONBLOCK) < 0)
    throw std::runtime_error("fcntl() - " + std::string(strerror(errno)));

  struct epoll_event event;
  event.events = EPOLLIN | EPOLLET;
  event.data.fd = client;
  if (epoll_ctl(communication_data_.epoll_fd, EPOLL_CTL_ADD, client, &event) <
      0)
    throw std::runtime_error("epoll_ctl() - " + std::string(strerror(errno)));

  log::info("new connection processed", __PRETTY_FUNCTION__);
}

void Server::Implementation::process_existing_connection(
    const descriptor &client_fd) {
  char buffer[1024] = {0};
  const std::string kAnswer("Message from server");

  int read_symbols_count(0);
  try {
    read_symbols_count = read(client_fd, buffer, 1024);
  } catch (const std::exception &exception) {
    throw std::runtime_error("read() - " + std::string(strerror(errno)));
  }

  if (read_symbols_count < 0)
    throw std::runtime_error("read() - " + std::string(strerror(errno)));

  log::info("client_fd(" + std::to_string(client_fd) +
                ") read message: " + buffer,
            __PRETTY_FUNCTION__);

  int written_symbols_count(0);
  try {
    written_symbols_count = write(client_fd, kAnswer.c_str(), kAnswer.length());
  } catch (const std::exception &exception) {
    throw std::runtime_error("write() - " + std::string(strerror(errno)));
  }

  log::info("written to client_fd(" + std::to_string(client_fd) +
                ") message: " + kAnswer.substr(0, written_symbols_count),
            __PRETTY_FUNCTION__);
}

void Server::Implementation::close_connection(const descriptor &client_fd) {
  log::info("closing connection(" + std::to_string(client_fd) + ")...",
            __PRETTY_FUNCTION__);

  if (epoll_ctl(communication_data_.epoll_fd, EPOLL_CTL_DEL, client_fd,
                nullptr) < 0)
    throw std::runtime_error("epoll_ctl() - " + std::string(strerror(errno)));

  close(client_fd);

  log::info("connection (" + std::to_string(client_fd) + ") closed",
            __PRETTY_FUNCTION__);
}

Server::Server() : impl_(new Implementation) {}

void Server::start(const std::string &ip, const uint32_t port) {
  impl_->start(ip, port);
}

void Server::stop() { impl_->stop(); }

} // namespace network
