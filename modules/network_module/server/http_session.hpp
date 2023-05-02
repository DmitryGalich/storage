#pragma once

#include <memory>
#include <functional>
#include <utility>
#include <string>
#include <list>

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/dynamic_body.hpp>

#include "../network_module_common.hpp"

class HttpSession : public std::enable_shared_from_this<HttpSession>
{
public:
    HttpSession() = delete;
    HttpSession(boost::asio::ip::tcp::socket socket,
                std::map<network_module::Url, network_module::Callback> callbacks);
    ~HttpSession() = default;

    void start();

private:
    void read_request();
    void do_request();
    void create_response();
    void write_response();
    void check_deadline();

private:
    std::map<network_module::Url, network_module::Callback> callbacks_;

    boost::asio::ip::tcp::socket socket_;
    boost::beast::flat_buffer buffer_{8192};
    boost::beast::http::request<boost::beast::http::dynamic_body> request_;
    boost::beast::http::response<boost::beast::http::dynamic_body> response_;
    boost::asio::steady_timer deadline_;
};
