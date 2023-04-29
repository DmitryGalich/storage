#include "../network_module.hpp"

namespace network_module
{
    namespace client
    {
        Client::Client() : client_impl_(std::make_unique<ClientImpl>()) {}

        Client::~Client() {}

        bool Client::start(const Config &config)
        {
            return true;
        }

        void Client::stop() {}
    }
}
