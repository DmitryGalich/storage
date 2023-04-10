#include <gtest/gtest.h>

#include "../network_module.hpp"

#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP

#include <thread>

class ServerTests : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST(Server, SimpleTest)
{
    network_module::Server::Config server_config;

    const auto kProcessorsCores = std::thread::hardware_concurrency();
    const auto kProcessorsCoresForServer = (kProcessorsCores > 1) ? (kProcessorsCores - 1) : 1;

    network_module::Server server(kProcessorsCoresForServer);

    EXPECT_TRUE(server.start({}));
    server.stop();
}
