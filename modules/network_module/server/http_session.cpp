#include "http_session.hpp"

#include "easylogging++.h"

#include "boost/beast.hpp"
#include "boost/beast/core.hpp"
#include "boost/beast/http.hpp"
#include "boost/beast/version.hpp"
#include "boost/asio.hpp"
#include "boost/bind.hpp"

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <string>

#include "websocket_session.hpp"

namespace
{
    bool is_error_important(const boost::system::error_code &error_code)
    {
        return !(error_code == boost::asio::error::operation_aborted);
    }
}

HttpSession::HttpSession(boost::asio::ip::tcp::socket socket,
                         SessionsManager &session_manager,
                         boost::asio::io_context &io_context,
                         const network_module::server::Server::Config::Callbacks callbacks)
    : socket_(std::move(socket)),
      kCallbacks_(callbacks),
      deadline_(socket_.get_executor(),
                std::chrono::seconds(60)),
      session_manager_(session_manager),
      io_context_(io_context)
{
}

HttpSession::~HttpSession()
{
}

void HttpSession::start()
{
    read();
    check_deadline();
}

void HttpSession::read()
{
    boost::beast::http::async_read(
        socket_,
        buffer_,
        request_,
        boost::bind(&HttpSession::on_read,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void HttpSession::on_read(boost::beast::error_code error_code,
                          std::size_t bytes_transferred)
{
    if (error_code)
    {
        LOG(ERROR) << error_code.value() << " : " << error_code.message();

        if (is_error_important(error_code))
            socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send, error_code);

        return;
    }

    if (boost::beast::websocket::is_upgrade(request_))
    {
        LOG(DEBUG) << "Request to update to websocket("
                   << socket_.remote_endpoint().address().to_string()
                   << ":"
                   << std::to_string(socket_.remote_endpoint().port())
                   << ")";

        auto session = std::make_shared<WebSocketSession>(std::move(socket_),
                                                          session_manager_,
                                                          io_context_,
                                                          kCallbacks_.web_sockets_callbacks_);
        session->start(std::move(request_), session);

        return;
    }

    do_request_responce();
}

void HttpSession::do_request_responce()
{
    response_.version(request_.version());
    response_.keep_alive(false);

    switch (request_.method())
    {

    case boost::beast::http::verb::get:
    {
        response_.result(boost::beast::http::status::ok);
        response_.set(boost::beast::http::field::server, "Beast");
        create_response();
        break;
    }
    default:
    {
        response_.result(boost::beast::http::status::bad_request);
        response_.set(boost::beast::http::field::content_type, "text/plain");
        boost::beast::ostream(response_.body())
            << "Invalid request-method '"
            << std::string(request_.method_string())
            << "'";
        break;
    }
    }

    write();
}

void HttpSession::create_response()
{
    response_.set(boost::beast::http::field::content_type, "text/html");

    const auto kPosition = kCallbacks_.http_callbacks_.find(static_cast<network_module::Url>(request_.target()));
    if (kPosition != kCallbacks_.http_callbacks_.end())
    {
        boost::beast::ostream(response_.body()) << kPosition->second();
    }
    else
    {
        boost::beast::ostream(response_.body()) << kCallbacks_.http_callbacks_.at(network_module::Urls::kPageNotFound_)();
    }
}

void HttpSession::write()
{
    response_.content_length(response_.body().size());

    boost::beast::http::async_write(
        socket_,
        response_,
        boost::bind(&HttpSession::on_write,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void HttpSession::on_write(boost::beast::error_code error_code,
                           std::size_t bytes_transferred)
{
    if (error_code)
    {
        LOG(ERROR) << error_code.value() << " : " << error_code.message();

        if (is_error_important(error_code))
            socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send, error_code);
    }

    deadline_.cancel();
}

void HttpSession::check_deadline()
{
    auto self = shared_from_this();

    deadline_.async_wait(
        [self](boost::beast::error_code error_code)
        {
            if (error_code)
            {
                if (is_error_important(error_code))
                    LOG(ERROR) << error_code.value() << " : " << error_code.message();

                return;
            }

            self->socket_.close(error_code);
        });
}