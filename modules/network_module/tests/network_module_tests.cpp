#include <gtest/gtest.h>

#include <thread>

#include "../configs/cmake_config.h"
#include "../network_module.hpp"

#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP

class ServerTests : public ::testing::Test
{
public:
    const std::string kConfigPath{CMAKE_CURRENT_SOURCE_DIR +
                                  std::string{"/configs/server_config.json"}};

protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ServerTests, ConfigLoading)
{
    LOG(INFO) << CMAKE_CURRENT_SOURCE_DIR;

    const std::string kAbstractServerConfigPath{
        CMAKE_CURRENT_SOURCE_DIR + std::string{"/configs/abstract_server_config.json"}};

    const auto kLoadedConfig =
        network_module::server::Server::Config::load_config(kAbstractServerConfigPath);

    EXPECT_EQ(kLoadedConfig.host_, "123.456.789.0");
    EXPECT_EQ(kLoadedConfig.port_, 1234);
}

TEST_F(ServerTests, SimpleTest)
{
    network_module::server::Server::Config server_config;

    const auto kProcessorsCores = std::thread::hardware_concurrency();
    const auto kProcessorsCoresForServer = (kProcessorsCores > 1) ? (kProcessorsCores - 1) : 1;
    const auto kConfig = network_module::server::Server::Config::load_config(kConfigPath);

    network_module::server::Server server(kProcessorsCoresForServer);
    EXPECT_TRUE(server.start(kConfig));
    server.stop();
}
