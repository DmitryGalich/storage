#include "http_session.hpp"

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

void HttpSession::process_fail(boost::system::error_code &error_code,
                               char const *reason) {}

void HttpSession::process_read(boost::system::error_code &error_code,
                               std::size_t)
{
    if (error_code == boost::beast::http::error::end_of_stream)
    {
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send,
                         error_code);
        return;
    }
}

void HttpSession::process_write(boost::system::error_code &error_code,
                                std::size_t,
                                bool is_need_close) {}
