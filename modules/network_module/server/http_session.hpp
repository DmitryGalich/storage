#pragma once

#include <memory>
#include <functional>
#include <utility>
#include <string>
#include <list>
#include <map>

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/dynamic_body.hpp>

#include "../network_module_common.hpp"

#include "sessions_manager.hpp"

class HttpSession : public std::enable_shared_from_this<HttpSession>
{
public:
    HttpSession() = delete;
    HttpSession(boost::asio::ip::tcp::socket socket,
                SessionsManager &session_manager,
                boost::asio::io_context &io_context,
                std::map<network_module::Url, network_module::HttpCallback> callbacks);
    ~HttpSession();

    void start();

private:
    void read();
    void on_read(boost::beast::error_code error_code,
                 std::size_t bytes_transferred);

    void write();
    void on_write(boost::beast::error_code error_code,
                  std::size_t bytes_transferred);

    void do_request_responce();
    void create_response();
    void check_deadline();

private:
    std::map<network_module::Url, network_module::HttpCallback> callbacks_;

    boost::asio::ip::tcp::socket socket_;
    boost::beast::flat_buffer buffer_{8192};
    boost::beast::http::request<boost::beast::http::dynamic_body> request_;
    boost::beast::http::response<boost::beast::http::dynamic_body> response_;
    boost::asio::steady_timer deadline_;

    SessionsManager &session_manager_;

    boost::asio::io_context &io_context_;
};
