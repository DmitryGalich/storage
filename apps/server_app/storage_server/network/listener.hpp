#pragma once

#include <memory>
#include <string>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>

// class shared_state;

class Listener : public std::enable_shared_from_this<Listener>
{
public:
    Listener() = delete;
    Listener(
        boost::asio::io_context &io_context,
        const boost::asio::ip::tcp::endpoint &endpoint);
    ~Listener() = default;

    void run();

private:
    void process_fail(boost::system::error_code error_code, char const *what);
    void process_accept(boost::system::error_code error_code);

private:
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
};