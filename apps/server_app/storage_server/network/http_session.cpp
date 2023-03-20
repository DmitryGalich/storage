#include "http_session.hpp"

#include "easylogging++.h"

namespace
{
    bool is_error_important(const boost::system::error_code &error_code)
    {
        return !(error_code == boost::asio::error::operation_aborted);
    }
}

HttpSession::HttpSession(
    boost::asio::ip::tcp::socket &socket)
    : socket_(socket)
{
    LOG(DEBUG) << "Constructing";
}

HttpSession::~HttpSession()
{
    LOG(DEBUG) << "Destructing";
}

void HttpSession::run()
{
    LOG(INFO) << "Socket connection(" << socket_.remote_endpoint().address().to_string() << " , " << socket_.remote_endpoint().port()
              << ") started";

    boost::beast::http::async_read(socket_, buffer_, request_,
                                   [&](boost::system::error_code error_code,
                                       std::size_t bytes)
                                   {
                                       process_read(error_code, bytes);
                                   });
}

void HttpSession::process_read(boost::system::error_code &error_code,
                               std::size_t)
{
    if (error_code)
    {
        if (error_code == boost::beast::http::error::end_of_stream)
        {
            LOG(INFO) << "Socket connection(" << socket_.remote_endpoint().address().to_string() << " , " << socket_.remote_endpoint().port()
                      << ") stopped";

            socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send,
                             error_code);
            return;
        }

        if (is_error_important(error_code))
        {
            LOG(ERROR) << "Code(" << error_code.value() << ") - "
                       << error_code.message();
            return;
        }
    }

    LOG(INFO) << "Reading logic";

    boost::beast::http::async_read(socket_, buffer_, request_,
                                   [&](boost::system::error_code error_code,
                                       std::size_t bytes)
                                   {
                                       process_read(error_code, bytes);
                                   });
}

void HttpSession::process_write(boost::system::error_code &error_code,
                                std::size_t,
                                bool is_need_close) {}
