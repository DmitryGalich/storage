#pragma once

#include <string>

namespace cloud
{
    namespace internal
    {
        class AbstractClient;

        AbstractClient *create_client(const std::string &config_path);
    }
}
