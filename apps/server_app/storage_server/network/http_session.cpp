#include "http_session.hpp"

#include "easylogging++.h"

HttpSession::HttpSession(SessionsManager &sessions_manager,
                         boost::asio::ip::tcp::socket &socket)
    : sessions_manager_(sessions_manager),
      socket_(socket) {}

void HttpSession::run()
{
    boost::beast::http::async_read(socket_, buffer_, request_,
                                   [&](boost::system::error_code error_code,
                                       std::size_t bytes)
                                   {
                                       process_read(error_code, bytes);
                                   });
}

void HttpSession::process_fail(const boost::system::error_code &error_code,
                               char const *reason)
{
    if (error_code == boost::asio::error::operation_aborted)
        return;

    LOG(ERROR) << reason << " : " << error_code.message();
}

void HttpSession::process_read(boost::system::error_code &error_code,
                               std::size_t)
{
    if (error_code == boost::beast::http::error::end_of_stream)
    {
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send,
                         error_code);
        return;
    }

    if (error_code)
    {
        process_fail(error_code, "read");
        return;
    }
}

void HttpSession::process_write(boost::system::error_code &error_code,
                                std::size_t,
                                bool is_need_close) {}
