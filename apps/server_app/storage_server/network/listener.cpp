#include "listener.hpp"

using namespace boost::system;
using namespace boost::asio;

Listener::Listener(io_context &io_context,
                   const ip::tcp::endpoint &endpoint)
    : acceptor_(io_context),
      socket_(io_context)
{
}

void Listener::run()
{
}

void Listener::process_fail(boost::system::error_code error_code, char const *what) {}

void Listener::process_accept(boost::system::error_code error_code) {}