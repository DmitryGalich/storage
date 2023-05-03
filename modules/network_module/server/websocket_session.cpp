#include "websocket_session.hpp"

#include "easylogging++.h"

#include "boost/asio/buffer.hpp"

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

WebSocketSession::~WebSocketSession()
{
}

void WebSocketSession::do_accept(boost::system::error_code error_code)
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
    // websocket_.async_read(
    //     buffer_,
    //     [self = shared_from_this()](
    //         boost::system::error_code error_code, std::size_t bytes)
    //     {
    //         self->do_read(error_code, bytes);
    //     });

    websocket_.async_read(
        buffer_,
        std::bind(
            &WebSocketSession::do_read,
            this,
            std::placeholders::_1,
            std::placeholders::_2));
}

void WebSocketSession::do_read(boost::system::error_code error_code,
                               std::size_t bytes_transferred)
{
    if (error_code)
    {
        if (is_error_important(error_code))
            LOG(ERROR) << "read - (" << error_code.value() << ") " << error_code.message();

        return;
    }

    const std::string kDataString(boost::asio::buffer_cast<const char *>(buffer_.data()), buffer_.size());

    LOG(INFO) << "Received message: " << kDataString;

    buffer_.consume(buffer_.size()); // Clear buffer

    prepare_for_reading();
}

void WebSocketSession::send() {}

void WebSocketSession::do_write(boost::system::error_code error_code,
                                std::size_t bytes_transferred)
{
}
