#pragma once

#include <string>

namespace network_module
{
    struct Config
    {
        std::string host_{"127.0.0.1"};
        int port_{8080};
    };

}
