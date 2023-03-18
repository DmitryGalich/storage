#include "http_session.hpp"

HttpSession::HttpSession(SessionsManager &sessions_manager,
                         boost::asio::ip::tcp::socket &socket)
    : sessions_manager_(sessions_manager),
      socket_(socket) {}

void HttpSession::run() {}

void HttpSession::process_fail(const boost::system::error_code &error_code,
                               char const *reason) {}

void HttpSession::process_read(const boost::system::error_code &eerror_codec,
                               std::size_t) {}

void HttpSession::process_write(const boost::system::error_code &error_code,
                                std::size_t,
                                bool is_need_close) {}
