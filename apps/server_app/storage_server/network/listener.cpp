#include "listener.hpp"

#define BOOST_BIND_NO_PLACEHOLDERS
#include <boost/asio/basic_socket_acceptor.hpp>
#include <boost/system/error_code.hpp>
#include <boost/bind.hpp>
#include <boost/asio/strand.hpp>

#include "easylogging++.h"

#include "http_session.hpp"

using namespace boost::asio;

namespace
{
  bool is_error_important(const boost::system::error_code &error_code)
  {
    return !(error_code == boost::asio::error::operation_aborted);
  }
}

Listener::Listener(const int &available_processors_cores,
                   io_context &io_context)
    : kAvailableProcessorsCores_(available_processors_cores),
      io_context_(io_context),
      acceptor_(io_context),
      socket_(io_context)
{
}

bool Listener::run(const ip::tcp::endpoint &endpoint)
{
  boost::system::error_code error_code;

  acceptor_.open(endpoint.protocol(), error_code);
  if (error_code)
  {
    LOG(ERROR) << "Code(" << error_code.value() << ") - "
               << error_code.message();
    return false;
  }

  acceptor_.set_option(socket_base::reuse_address(true), error_code);
  if (error_code)
  {
    LOG(ERROR) << "Code(" << error_code.value() << ") - "
               << error_code.message();
    return false;
  }

  acceptor_.bind(endpoint, error_code);
  if (error_code)
  {
    LOG(ERROR) << "Code(" << error_code.value() << ") - "
               << error_code.message();
    return false;
  }

  acceptor_.listen(
      socket_base::max_listen_connections, error_code);
  if (error_code)
  {
    LOG(ERROR) << "Code(" << error_code.value() << ") - "
               << error_code.message();
    return false;
  }

  prepare_for_accept();

  return true;
}

void Listener::prepare_for_accept()
{
  acceptor_.async_accept(
      socket_, [&](const boost::system::error_code &error_code)
      { process_accept(error_code); });

  // acceptor_.async_accept(
  //     make_strand(io_context_),
  //     boost::beast::bind_front_handler(
  //         &Listener::process_accept,
  //         shared_from_this()));
}

void Listener::process_accept(const boost::system::error_code &error_code)
{
  if (error_code)
  {
    if (is_error_important(error_code))
    {
      LOG(ERROR) << "Code(" << error_code.value() << ") - "
                 << error_code.message();
      return;
    }
  }

  sessions_manager_.add(std::make_shared<HttpSession>(socket_));

  prepare_for_accept();
}
