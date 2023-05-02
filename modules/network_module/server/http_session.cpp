#include "http_session.hpp"

#include "easylogging++.h"

#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
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
                         std::map<network_module::Url, network_module::Callback> callbacks)
    : socket_(std::move(socket)),
      callbacks_(callbacks),
      deadline_(socket_.get_executor(),
                std::chrono::seconds(60))
{
}

void HttpSession::start()
{
    read_request();
    check_deadline();
}

void HttpSession::read_request()
{
    auto self = shared_from_this();

    boost::beast::http::async_read(
        socket_,
        buffer_,
        request_,
        [self](boost::beast::error_code error_code,
               std::size_t bytes_transferred)
        {
            boost::ignore_unused(bytes_transferred);

            if (error_code)
            {
                if (is_error_important(error_code))
                {
                    LOG(ERROR) << "async_read - (" << error_code.value() << ") " << error_code.message();
                    self->socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send, error_code);
                }
                return;
            }

            if (boost::beast::websocket::is_upgrade(self->request_))
            {
                LOG(INFO) << "Request to update to websocket("
                          << self->socket_.remote_endpoint().address().to_string()
                          << ":"
                          << std::to_string(self->socket_.remote_endpoint().port())
                          << ")";

                std::make_shared<WebSocketSession>(std::move(self->socket_))
                    ->run(std::move(self->request_));
                return;
            }

            self->do_request();
        });
}

void HttpSession::do_request()
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

    write_response();
}

void HttpSession::create_response()
{
    response_.set(boost::beast::http::field::content_type, "text/html");

    const auto kPosition = callbacks_.find(static_cast<network_module::Url>(request_.target()));
    if (kPosition != callbacks_.end())
    {
        boost::beast::ostream(response_.body()) << kPosition->second();
    }
    else
    {
        boost::beast::ostream(response_.body()) << callbacks_.at(network_module::Urls::kPageNotFound_)();
    }
}

void HttpSession::write_response()
{
    auto self = shared_from_this();

    response_.content_length(response_.body().size());

    boost::beast::http::async_write(
        socket_,
        response_,
        [self](boost::beast::error_code error_code, std::size_t)
        {
            // Is need shutdown here?

            if (error_code)
            {
                if (is_error_important(error_code))
                {
                    LOG(ERROR) << "async_read - (" << error_code.value() << ") " << error_code.message();
                    self->socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send, error_code);
                }
            }

            self->deadline_.cancel();
        });
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
                    LOG(ERROR) << "async_read - (" << error_code.value() << ") " << error_code.message();

                return;
            }

            self->socket_.close(error_code);
        });
}