#include "websocket_session.hpp"

#include "easylogging++.h"

#include "boost/asio/buffer.hpp"

#include <memory>

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
                                   boost::asio::io_context &io_context,
                                   const ReceivingCallback callback)
    : kReadingCallback_(callback),
      session_manager_(session_manager),
      websocket_(std::move(socket)),
      io_context_(io_context),
      acception_deadline_timer_(io_context_, boost::posix_time::seconds(5))
{
}

WebSocketSession::~WebSocketSession()
{
    LOG(DEBUG);
    itself_.reset();
}

void WebSocketSession::on_acception_timer(boost::system::error_code error_code)
{
    LOG(DEBUG);
    itself_.reset();
}

void WebSocketSession::do_accept(boost::system::error_code error_code)
{
    if (error_code)
    {
        if (is_error_important(error_code))
        {
            LOG(ERROR) << "accept - (" << error_code.value() << ") " << error_code.message();
            return;
        }
    }

    acception_deadline_timer_.cancel();

    if (!session_manager_.add(itself_))
    {
        LOG(ERROR) << "Can't add websocket session";
        return;
    }
    itself_.reset();

    prepare_for_reading();
}

void WebSocketSession::prepare_for_reading()
{
    websocket_.async_read(
        buffer_,
        boost::bind(
            &WebSocketSession::on_read,
            this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void WebSocketSession::on_read(boost::system::error_code error_code,
                               std::size_t bytes_transferred)
{
    if (error_code)
    {
        if (is_error_important(error_code))
        {
            LOG(ERROR) << "do_receive - (" << error_code.value() << ") " << error_code.message();
            return;
        }
    }

    const std::string kDataString(boost::asio::buffer_cast<const char *>(buffer_.data()), buffer_.size());

    LOG(DEBUG) << "Received message: " << kDataString;

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
        {
            LOG(ERROR) << "do_write - (" << error_code.value() << ") " << error_code.message();
            return;
        }
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
