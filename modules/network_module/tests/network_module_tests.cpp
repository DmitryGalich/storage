#include <gtest/gtest.h>

#include "../network_module.hpp"

#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP

#include <thread>

TEST(Server, SimpleTest)
{
    network_module::Server::Config server_config;

    const auto kProcessorsCores = std::thread::hardware_concurrency();
    const auto kProcessorsCoresForServer = (kProcessorsCores > 1) ? (kProcessorsCores - 1) : 1;

    network_module::Server server(kProcessorsCoresForServer);
}
