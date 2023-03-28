#include "websocket_session.hpp"

namespace
{
    bool is_error_important(const boost::system::error_code &error_code)
    {
        return !((error_code == boost::asio::error::operation_aborted) ||
                 (error_code == boost::beast::websocket::error::closed));
    }
}

WebSocketSession::WebSocketSession(boost::asio::ip::tcp::socket socket)
    : websocket_(std::move(socket))
{
}

void WebSocketSession::process_accept(boost::system::error_code error_code)
{
    if (error_code)
    {
        if (is_error_important(error_code))
            LOG(ERROR) << "accept - (" << error_code.value() << ") " << error_code.message();

        return;
    }

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
