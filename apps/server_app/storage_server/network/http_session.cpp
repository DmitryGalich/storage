#include "http_session.hpp"

#include "easylogging++.h"

#include <chrono>

#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>

namespace
{
    bool is_error_important(const boost::system::error_code &error_code)
    {
        return !(error_code == boost::asio::error::operation_aborted);
    }
}

namespace my_program_state
{
    std::size_t request_count()
    {
        static std::size_t count = 0;
        return ++count;
    }

    std::time_t now()
    {
        return std::time(0);
    }
}

HttpSession::HttpSession(boost::asio::ip::tcp::socket socket)
    : socket_(std::move(socket)),
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
            }
            else
            {
                if (boost::beast::websocket::is_upgrade(self->request_))
                {
                    LOG(INFO) << "Socket request " +
                                     self->socket_.remote_endpoint().address().to_string() +
                                     ":" +
                                     std::to_string(self->socket_.remote_endpoint().port());
                }
                else
                {
                    self->process_request();
                }
            }
        });
}

void HttpSession::process_request()
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
    if (request_.target() == "/count")
    {
        response_.set(boost::beast::http::field::content_type,
                      "text/html");

        boost::beast::ostream(response_.body())
            << "<html>\n"
            << "<head><title>Request count</title></head>\n"
            << "<body>\n"
            << "<h1>Request count</h1>\n"
            << "<p>There have been "
            << my_program_state::request_count()
            << " requests so far.</p>\n"
            << "</body>\n"
            << "</html>\n";
    }
    else if (request_.target() == "/time")
    {
        response_.set(boost::beast::http::field::content_type,
                      "text/html");
        boost::beast::ostream(response_.body())
            << "<html>\n"
            << "<head><title>Current time</title></head>\n"
            << "<body>\n"
            << "<h1>Current time</h1>\n"
            << "<p>The current time is "
            << my_program_state::now()
            << " seconds since the epoch.</p>\n"
            << "</body>\n"
            << "</html>\n";
    }
    else
    {
        response_.result(boost::beast::http::status::not_found);
        response_.set(boost::beast::http::field::content_type, "text/plain");
        boost::beast::ostream(response_.body()) << "File not found\r\n";
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
            }
            else
            {
                self->socket_.close(error_code);
            }
        });
}