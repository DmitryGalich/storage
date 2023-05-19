#include <future>
#include <thread>
#include <chrono>

#include "configs/cmake_config.h"

#include "easylogging++.h"

#include "storage_client/storage_client.hpp"

INITIALIZE_EASYLOGGINGPP

void configure_logger()
{
    el::Configurations config(CMAKE_CURRENT_SOURCE_DIR + std::string{"/configs/log_config.conf"});
    el::Loggers::reconfigureAllLoggers(config);
}

bool wait_for_exit_command()
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
            return true;
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

    std::future<bool> future = std::async(&wait_for_exit_command);
    while (true)
    {
        if (future.wait_for(std::chrono::seconds(1)) == std::future_status::ready)
            break;
    }

    client.stop();

    return 0;
}
