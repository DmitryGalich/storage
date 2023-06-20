#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "boost/asio/ip/tcp.hpp"
#include "boost/beast/websocket/stream.hpp"
#include "boost/beast/http/dynamic_body.hpp"
#include "boost/beast/http/fields.hpp"
#include "boost/beast/websocket/impl/accept.hpp"
#include "boost/bind.hpp"
#include "boost/asio/placeholders.hpp"
#include "boost/asio.hpp"

#include "sessions_manager.hpp"

#include "../network_module.hpp"

class WebSocketSession : public std::enable_shared_from_this<WebSocketSession>
{
public:
    using ReceivingCallback = std::function<void(const std::string &)>;

    WebSocketSession() = delete;
    WebSocketSession(boost::asio::ip::tcp::socket socket,
                     SessionsManager &session_manager,
                     boost::asio::io_context &io_context,
                     const network_module::server::Server::Config::Callbacks::WebSocketsCallbacks callback);
    ~WebSocketSession();

    template <class Body, class Allocator>
    void start(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> request,
               std::shared_ptr<WebSocketSession> itself);

    void send(std::shared_ptr<std::string const> const &data);

private:
    void do_accept(boost::system::error_code error_code);
    void prepare_for_reading();
    void on_read(boost::system::error_code error_code, std::size_t bytes_transferred);
    void do_write(boost::system::error_code error_code, std::size_t bytes_transferred);

    void on_acception_timer(boost::system::error_code error_code);

    void stop();

private:
    const network_module::server::Server::Config::Callbacks::WebSocketsCallbacks kCallbacks_;

    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> websocket_;
    boost::beast::flat_buffer buffer_;
    std::vector<std::shared_ptr<std::string const>> queue_;

    SessionsManager &session_manager_;

    boost::asio::io_context &io_context_;
    boost::asio::deadline_timer acception_deadline_timer_;

    std::shared_ptr<WebSocketSession> itself_;
};

template <class Body, class Allocator>
void WebSocketSession::start(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> request,
                             std::shared_ptr<WebSocketSession> itself)
{
    itself_ = itself;

    websocket_.async_accept(
        request,
        boost::bind(
            &WebSocketSession::do_accept,
            this,
            boost::asio::placeholders::error));

    acception_deadline_timer_.async_wait(
        boost::bind(&WebSocketSession::on_acception_timer,
                    this,
                    boost::asio::placeholders::error));
}