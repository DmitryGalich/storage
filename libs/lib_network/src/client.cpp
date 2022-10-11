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

    void write_message(const std::string &message);
    std::string read_message();
  };

  void Client::Implementation::start(const std::string &server_ip,
                                     const uint32_t server_port)
  {
    communication_data_.clear();
    communication_data_.ip_ = server_ip;
    communication_data_.port_ = server_port;

    logger::info("starting...", __PRETTY_FUNCTION__);

    communication_data_.thread_.reset(
        new std::thread([&]()
                        { run_communicating_thread(); }));

    logger::info("started", __PRETTY_FUNCTION__);
  }

  void Client::Implementation::stop()
  {
    logger::info("stopping...", __PRETTY_FUNCTION__);

    if (communication_data_.thread_)
    {
      communication_data_.is_need_connecting_cycling_.store(false);
      communication_data_.is_need_communication_cycling_.store(false);

      communication_data_.thread_->join();

      communication_data_.clear();
    }

    logger::info("stopped", __PRETTY_FUNCTION__);
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
        logger::error("communication thread error(" + std::string(exception.what()) +
                          ")",
                      __PRETTY_FUNCTION__);
      }

      deactivate_socket();

      std::this_thread::sleep_for(500ms);
    }
  }

  void Client::Implementation::configure_socket()
  {
    logger::info("configuring socket(" + communication_data_.ip_ + ", " +
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

    logger::info("socket configured(" + communication_data_.ip_ + ", " +
                     std::to_string(communication_data_.port_) + ")",
                 __PRETTY_FUNCTION__);
  }

  void Client::Implementation::connect_to_server()
  {
    logger::info("connecting to server(" + communication_data_.ip_ + ", " +
                     std::to_string(communication_data_.port_) + ")...",
                 __PRETTY_FUNCTION__);

    if (connect(communication_data_.server_,
                (struct sockaddr *)&communication_data_.server_address_data_,
                sizeof(communication_data_.server_address_data_)) < 0)
      throw std::runtime_error("connect() - " + std::string(strerror(errno)));

    logger::info("connected to server(" +
                     std::to_string(communication_data_.server_) + ")",
                 __PRETTY_FUNCTION__);
  }

  void Client::Implementation::run_communication_cycle()
  {
    using namespace std::chrono_literals;

    static const std::string kMessageBase("Message from client");
    int iteration_number(0);

    communication_data_.is_need_communication_cycling_.store(true);

    while (communication_data_.is_need_communication_cycling_.load())
    {

      write_message(kMessageBase + " " +
                    std::to_string(iteration_number++));

      LOG_INFO(kMessageBase + " " +
               std::to_string(iteration_number++));

      auto readed_string = read_message();
      LOG_INFO(readed_string);

      std::this_thread::sleep_for(500ms);
    }

    communication_data_.is_need_communication_cycling_.store(false);
  }

  void Client::Implementation::deactivate_socket()
  {
    if (communication_data_.server_ >= 0)
    {
      logger::info("closing connection(" +
                       std::to_string(communication_data_.server_) + ")...",
                   __PRETTY_FUNCTION__);

      close(communication_data_.server_);

      logger::info("connection closed(" +
                       std::to_string(communication_data_.server_) + ")",
                   __PRETTY_FUNCTION__);
    }
  }

  void Client::Implementation::write_message(const std::string &message)
  {
    static int written_symbols_count(0);

    try
    {
      written_symbols_count =
          write(communication_data_.server_, message.c_str(), message.length());
    }
    catch (const std::exception &exception)
    {
      throw std::runtime_error("write() - " + std::string(strerror(errno)));
    }

    if (written_symbols_count < 0)
    {
      throw std::runtime_error("written_symbols_count is negative");
    }
  }

  std::string Client::Implementation::read_message()
  {
    static const uint kBufferSize(1024);
    static int read_symbols_count(0);

    static char buffer[kBufferSize] = {0};
    read_symbols_count = 0;

    memset(buffer, 0, sizeof buffer);

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

    return buffer;
  }

  Client::Client() : impl_(new Implementation) {}

  void Client::start(const std::string &ip, const uint32_t port)
  {
    impl_->start(ip, port);
  }

  void Client::stop() { impl_->stop(); }

} // namespace network
