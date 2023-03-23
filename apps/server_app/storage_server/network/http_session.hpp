#pragma once

#include <memory>

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/dynamic_body.hpp>

class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
public:
    HttpConnection() = delete;
    HttpConnection(boost::asio::ip::tcp::socket socket);
    ~HttpConnection() = default;

    void start();

private:
    void read_request();
    void process_request();
    void create_response();
    void write_response();
    void check_deadline();

private:
    boost::asio::ip::tcp::socket socket_;
    boost::beast::flat_buffer buffer_{8192};
    boost::beast::http::request<boost::beast::http::dynamic_body> request_;
    boost::beast::http::response<boost::beast::http::dynamic_body> response_;
    boost::asio::steady_timer deadline_;
};
