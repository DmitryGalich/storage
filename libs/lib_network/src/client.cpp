#include "network.h"

#include <arpa/inet.h>
#include <atomic>
#include <cstring>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include "logger.h"

namespace network
{

  class Client::Implementation
  {
  public:
    Implementation() = default;
    ~Implementation() { stop(); };

    void start(const std::string &server_ip, const uint32_t server_port);
    void stop();

  private:
    struct CommunicationData
    {
      std::unique_ptr<std::thread> thread_;
      std::atomic_bool is_need_connecting_cycling_{false};
      std::atomic_bool is_need_communication_cycling_{false};

      descriptor server_{kInvalidDesciptor};
      struct sockaddr_in server_address_data_;

      std::string ip_;
      uint32_t port_;

      void clear()
      {
        is_need_connecting_cycling_.store(false);
        is_need_communication_cycling_.store(false);

        thread_.reset();

        ip_.clear();
        port_ = 0;

        server_ = kInvalidDesciptor;
      }
    } communication_data_;

    void run_communicating_thread();
    void configure_socket();
    void connect_to_server();
    void run_communication_cycle();
    void deactivate_socket();
  };

  void Client::Implementation::start(const std::string &server_ip,
                                     const uint32_t server_port)
  {
    communication_data_.clear();
    communication_data_.ip_ = server_ip;
    communication_data_.port_ = server_port;

    LOG_INFO("starting...");

    communication_data_.thread_.reset(
        new std::thread([&]()
                        { run_communicating_thread(); }));

    LOG_INFO("started");
  }

  void Client::Implementation::stop()
  {
    LOG_INFO("stopping...");

    if (communication_data_.thread_)
    {
      communication_data_.is_need_connecting_cycling_.store(false);
      communication_data_.is_need_communication_cycling_.store(false);

      communication_data_.thread_->join();

      communication_data_.clear();
    }

    LOG_INFO("stopped");
  }

  void Client::Implementation::run_communicating_thread()
  {
    using namespace std::chrono_literals;

    communication_data_.is_need_connecting_cycling_.store(true);

    while (communication_data_.is_need_connecting_cycling_.load())
    {
      try
      {
        configure_socket();
        connect_to_server();
        run_communication_cycle();
      }
      catch (const std::exception &exception)
      {
        LOG_ERROR("communication thread error(" + std::string(exception.what()) +
                      ")",
                  __PRETTY_FUNCTION__);
      }

      deactivate_socket();

      std::this_thread::sleep_for(500ms);
    }
  }

  void Client::Implementation::configure_socket()
  {
    LOG_INFO("configuring socket(" + communication_data_.ip_ + ", " +
                 std::to_string(communication_data_.port_) + ")...",
             __PRETTY_FUNCTION__);

    if ((communication_data_.server_ = socket(AF_INET, SOCK_STREAM, 0)) < 0)
      throw std::runtime_error("socket() - " + std::string(strerror(errno)));

    communication_data_.server_address_data_.sin_family = AF_INET;
    communication_data_.server_address_data_.sin_port =
        htons(communication_data_.port_);

    if (inet_pton(AF_INET, communication_data_.ip_.c_str(),
                  &communication_data_.server_address_data_.sin_addr) != 1)
      throw std::runtime_error("inet_pton() - " + std::string(strerror(errno)));

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
      throw std::runtime_error("signal() - " + std::string(strerror(errno)));

    LOG_INFO("socket configured(" + communication_data_.ip_ + ", " +
                 std::to_string(communication_data_.port_) + ")",
             __PRETTY_FUNCTION__);
  }

  void Client::Implementation::connect_to_server()
  {
    LOG_INFO("connecting to server(" + communication_data_.ip_ + ", " +
                 std::to_string(communication_data_.port_) + ")...",
             __PRETTY_FUNCTION__);

    if (connect(communication_data_.server_,
                (struct sockaddr *)&communication_data_.server_address_data_,
                sizeof(communication_data_.server_address_data_)) < 0)
      throw std::runtime_error("connect() - " + std::string(strerror(errno)));

    LOG_INFO("connected to server(" +
                 std::to_string(communication_data_.server_) + ")",
             __PRETTY_FUNCTION__);
  }

  void Client::Implementation::run_communication_cycle()
  {
    using namespace std::chrono_literals;

    static const std::string kMessageBase("Message from client");
    static const uint kBufferSize(1024);
    char buffer[kBufferSize] = {0};
    int iteration_number(0);
    int written_symbols_count(0);
    int read_symbols_count(0);

    communication_data_.is_need_communication_cycling_.store(true);

    while (communication_data_.is_need_communication_cycling_.load())
    {
      std::string message(kMessageBase + " " +
                          std::to_string(iteration_number++));

      try
      {
        written_symbols_count =
            write(communication_data_.server_, message.c_str(), message.length());
      }
      catch (const std::exception &exception)
      {
        throw std::runtime_error("write() - " + std::string(strerror(errno)));
      }

      LOG_INFO("wrote message: " + message.substr(0, written_symbols_count),
               __PRETTY_FUNCTION__);

      try
      {
        read_symbols_count =
            read(communication_data_.server_, buffer, kBufferSize);
      }
      catch (const std::exception &exception)
      {
        throw std::runtime_error("read() - " + std::string(strerror(errno)));
      }

      if (read_symbols_count < 0)
        throw std::runtime_error("read() - " + std::string(strerror(errno)));

      LOG_INFO(std::string("read message: ") + buffer);

      std::this_thread::sleep_for(500ms);
    }

    communication_data_.is_need_communication_cycling_.store(false);
  }

  void Client::Implementation::deactivate_socket()
  {
    if (communication_data_.server_ >= 0)
    {
      LOG_INFO("closing connection(" +
                   std::to_string(communication_data_.server_) + ")...",
               __PRETTY_FUNCTION__);

      close(communication_data_.server_);

      LOG_INFO("connection closed(" +
                   std::to_string(communication_data_.server_) + ")",
               __PRETTY_FUNCTION__);
    }
  }

  Client::Client() : impl_(new Implementation) {}

  void Client::start(const std::string &ip, const uint32_t port)
  {
    impl_->start(ip, port);
  }

  void Client::stop() { impl_->stop(); }

} // namespace network
