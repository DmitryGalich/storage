#include "listener.hpp"

#include <boost/asio/basic_socket_acceptor.hpp>
#include <boost/system/error_code.hpp>

#include "easylogging++.h"

#include "http_session.hpp"

using namespace boost::asio;

Listener::Listener(io_context &io_context)
    : acceptor_(io_context),
      socket_(io_context)
{
}

bool Listener::run(const ip::tcp::endpoint &endpoint)
{
  boost::system::error_code error_code;

  acceptor_.open(endpoint.protocol(), error_code);
  if (error_code)
  {
    process_fail(error_code, "open");
    return false;
  }

  acceptor_.set_option(socket_base::reuse_address(true));
  if (error_code)
  {
    process_fail(error_code, "set_option");
    return false;
  }

  acceptor_.bind(endpoint, error_code);
  if (error_code)
  {
    process_fail(error_code, "bind");
    return false;
  }

  acceptor_.listen(
      socket_base::max_listen_connections, error_code);
  if (error_code)
  {
    process_fail(error_code, "listen");
    return false;
  }

  prepare_for_accept();

  return true;
}

void Listener::prepare_for_accept()
{
  acceptor_.async_accept(
      socket_,
      [&](boost::system::error_code error_code)
      {
        process_accept(error_code);
      });
}

void Listener::process_accept(const boost::system::error_code &error_code)
{
  if (error_code)
    return process_fail(error_code, "accept");

  std::make_shared<HttpSession>(sessions_manager_, socket_)
      ->run();

  acceptor_.async_accept(
      socket_,
      [self = shared_from_this()](boost::system::error_code error_code)
      {
        self->process_accept(error_code);
      });
}

void Listener::process_fail(const boost::system::error_code &error_code, char const *reason)
{
  if (error_code == error::operation_aborted)
    return;

  LOG(ERROR) << reason << " : " << error_code.message();
}