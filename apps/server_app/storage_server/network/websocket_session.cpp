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
    LOG(DEBUG);
}

WebSocketSession::~WebSocketSession()
{
    LOG(DEBUG);
}

void WebSocketSession::process_accept(boost::system::error_code error_code)
{
    if (error_code)
    {
        if (is_error_important(error_code))
            LOG(ERROR) << "accept - (" << error_code.value() << ") " << error_code.message();

        return;
    }

    LOG(INFO) << "New websocket connection established";

    prepare_for_reading();
}

void WebSocketSession::prepare_for_reading()
{
    websocket_.async_read(
        buffer_,
        [self = shared_from_this()](
            boost::system::error_code error_code, std::size_t bytes)
        {
            self->process_read(error_code, bytes);
        });
}

void WebSocketSession::process_read(boost::system::error_code error_code,
                                    std::size_t bytes_transferred)
{
    LOG(INFO) << "Received message: " << buffer_.data();

    buffer_.consume(buffer_.size());

    prepare_for_reading();
}

void WebSocketSession::process_write(boost::system::error_code error_code,
                                     std::size_t bytes_transferred)
{
}
