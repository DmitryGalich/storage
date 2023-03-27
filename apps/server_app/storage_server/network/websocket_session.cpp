#include "websocket_session.hpp"

WebSocketSession::WebSocketSession(boost::asio::ip::tcp::socket socket)
    : websocket_(std::move(socket))
{
}

void WebSocketSession::process_accept()
{
    LOG(INFO) << "KEK";
}

// void WebSocketSession::process_fail(const error_code &error_code,
//                                     char const *reason) {}

// void WebSocketSession::process_read(const error_code &eerror_codec,
//                                     std::size_t) {}

// void WebSocketSession::process_write(const error_code &error_code,
//                                      std::size_t,
//                                      bool is_need_close) {}
