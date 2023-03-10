#pragma once

#include <string>

namespace cloud
{
    namespace internal
    {
        class AbstractServer;
        AbstractServer *create_server(const std::string &config_path);
    }
}
