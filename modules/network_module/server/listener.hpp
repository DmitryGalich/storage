#pragma once

#include <memory>
#include <string>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>

#include "sessions_manager.hpp"

class Listener : public std::enable_shared_from_this<Listener>
{
public:
    Listener() = delete;
    Listener(const int &available_processors_cores,
             boost::asio::io_context &io_context);
    ~Listener() = default;

    bool run(const boost::asio::ip::tcp::endpoint &endpoint);

private:
    void prepare_for_accepting();
    void do_accept(const boost::system::error_code &error_code);

private:
    const int &kAvailableProcessorsCores_;

    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::io_context &io_context_;

    SessionsManager sessions_manager_;
};