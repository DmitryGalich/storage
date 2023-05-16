#include <future>

#include "configs/cmake_config.h"

#include "easylogging++.h"

#include "storage_client/storage_client.hpp"

INITIALIZE_EASYLOGGINGPP

void configure_logger()
{
    el::Configurations config(CMAKE_CURRENT_SOURCE_DIR + std::string{"/configs/log_config.conf"});
    el::Loggers::reconfigureAllLoggers(config);
}

void wait_for_user_command()
{
    while (true)
    {
        std::string input;
        getline(std::cin, input);

        if (input == "q" ||
            input == "Q" ||
            input == "c" ||
            input == "C")
        {
            break;
        }
    }
}

int main()
{
    configure_logger();

    LOG(INFO) << "================================";
    LOG(INFO) << PROJECT_NAME;
    LOG(INFO) << "version " << PROJECT_VERSION;

    storage::client::Client client;

    try
    {
        if (!client.start(CMAKE_CURRENT_SOURCE_DIR + std::string{"/configs/client_config.json"}))
        {
            LOG(ERROR) << "Can't start client";
            LOG(INFO) << "Shutting down the application";
            client.stop();
            return -1;
        }
    }
    catch (const std::exception &e)
    {
        LOG(ERROR) << e.what();
        LOG(INFO) << "Shutting down the application";
        client.stop();
        return -1;
    }

    std::async(wait_for_user_command).get();

    client.stop();

    return 0;
}
