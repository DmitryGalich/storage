#pragma once

#include <string>

#include "abstract_client.h"

namespace cloud
{
    namespace internal
    {
        AbstractClient *create_client(const std::string &config_path);
    }
}
