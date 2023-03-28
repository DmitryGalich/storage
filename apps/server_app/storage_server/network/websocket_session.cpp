#include "websocket_session.hpp"

WebSocketSession::WebSocketSession(boost::asio::ip::tcp::socket socket)
    : websocket_(std::move(socket))
{
}

void WebSocketSession::process_accept(boost::system::error_code error_code)
{
    LOG(INFO) << "KEK";
}

void WebSocketSession::process_read(boost::system::error_code error_code,
                                    std::size_t bytes_transferred)
{
}

void WebSocketSession::process_write(boost::system::error_code error_code,
                                     std::size_t bytes_transferred)
{
}
