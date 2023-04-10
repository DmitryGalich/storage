#include "../network_module.hpp"

namespace network_module
{
    Client::Client() {}

    Client::~Client() {}

    bool Client::start(const Config &config)
    {
        return true;
    }

    void Client::stop() {}
}
