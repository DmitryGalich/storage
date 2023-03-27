#pragma once

#include <memory>

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/beast/http/dynamic_body.hpp>
#include <boost/beast/http/fields.hpp>
#include <boost/beast/websocket/impl/accept.hpp>

#include "easylogging++.h"

class WebSocketSession : public std::enable_shared_from_this<WebSocketSession>
{
public:
    WebSocketSession() = delete;
    WebSocketSession(boost::asio::ip::tcp::socket socket);
    ~WebSocketSession() = default;

    template <class Body, class Allocator>
    void run(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> request);

private:
    void process_accept();

private:
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> websocket_;
};

template <class Body, class Allocator>
void WebSocketSession::run(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> request)
{
    auto self = shared_from_this();

    // websocket_.async_accept(
    //     boost::asio::bind_executor(
    //         strand_,
    //         std::bind(
    //             &session::on_accept,
    //             shared_from_this(),
    //             std::placeholders::_1)));
}