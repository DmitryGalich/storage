#include <gtest/gtest.h>

#include "../network_module.hpp"

#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP

TEST(Server, SimpleTest)
{
    network_module::Server::Config server_config;

    int num = 1;
    network_module::Server server(num);
}
