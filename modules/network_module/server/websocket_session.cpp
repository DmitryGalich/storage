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

WebSocketSession::WebSocketSession(boost::asio::ip::tcp::socket socket,
                                   SessionsManager &session_manager,
                                   const ReceivingCallback callback)
    : kReadingCallback_(callback),
      session_manager_(session_manager),
      websocket_(std::move(socket))
{
}

WebSocketSession::~WebSocketSession()
{
    session_manager_.remove(this);
}

void WebSocketSession::do_accept(boost::system::error_code error_code)
{
    LOG(INFO) << "Do accept";

    if (error_code)
    {
        if (is_error_important(error_code))
            LOG(ERROR) << "accept - (" << error_code.value() << ") " << error_code.message();

        return;
    }

    session_manager_.add(this);

    LOG(INFO) << "New websocket connection established";

    prepare_for_reading();
}

void WebSocketSession::prepare_for_reading()
{
    websocket_.async_read(
        buffer_,
        std::bind(
            &WebSocketSession::do_receive,
            this,
            std::placeholders::_1,
            std::placeholders::_2));
}

void WebSocketSession::do_receive(boost::system::error_code error_code,
                               std::size_t bytes_transferred)
{
    if (error_code)
    {
        if (is_error_important(error_code))
            LOG(ERROR) << "do_receive - (" << error_code.value() << ") " << error_code.message();

        return;
    }

    const std::string kDataString(boost::asio::buffer_cast<const char *>(buffer_.data()), buffer_.size());

    LOG(INFO) << "Received message: " << kDataString;

    buffer_.consume(buffer_.size()); // Clear buffer

    prepare_for_reading();
}

void WebSocketSession::send(std::shared_ptr<std::string const> const &data)
{
    queue_.push_back(data);

    if (queue_.size() > 1)
        return;

    websocket_.async_write(
        boost::asio::buffer(*queue_.front()),
        [self = shared_from_this()](
            boost::system::error_code error_code, std::size_t bytes_transferred)
        {
            self->do_write(error_code, bytes_transferred);
        });
}

void WebSocketSession::do_write(boost::system::error_code error_code,
                                std::size_t bytes_transferred)
{
    if (error_code)
    {
        if (is_error_important(error_code))
            LOG(ERROR) << "do_write - (" << error_code.value() << ") " << error_code.message();

        return;
    }

    queue_.erase(queue_.begin());

    if (!queue_.empty())
        websocket_.async_write(
            boost::asio::buffer(*queue_.front()),
            [self = shared_from_this()](
                boost::system::error_code error_code,
                std::size_t bytes_transferred)
            {
                self->do_write(error_code, bytes_transferred);
            });
}
