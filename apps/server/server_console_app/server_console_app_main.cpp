#include <signal.h>
#include <thread>

#include "configs/cmake_config.h"

#include "storage_server/storage_server.hpp"

#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

void configure_logger()
{
    el::Configurations config(CMAKE_CURRENT_SOURCE_DIR + std::string{"/configs/log_config.conf"});
    el::Loggers::reconfigureAllLoggers(config);
}

int main()
{
    configure_logger();

    LOG(INFO) << "================================";
    LOG(INFO) << PROJECT_NAME;
    LOG(INFO) << "version " << PROJECT_VERSION;

    const auto kProcessorsCores = std::thread::hardware_concurrency();
    const auto kServerWorkersNumber = (kProcessorsCores > 1) ? (kProcessorsCores - 1) : 1;

    storage::server::Server server;

    try
    {
        if (!server.start(kServerWorkersNumber,
                          CMAKE_CURRENT_SOURCE_DIR + std::string{"/configs/server_config.json"},
                          CMAKE_CURRENT_SOURCE_DIR + std::string{"/web_pages/"}))
        {
            LOG(ERROR) << "Can't start server";
            LOG(INFO) << "Shutting down the application";
            server.stop();
            return -1;
        }
    }
    catch (const std::exception &e)
    {
        LOG(ERROR) << e.what();
        LOG(INFO) << "Shutting down the application";
        server.stop();
        return -1;
    }

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

    server.stop();

    return 0;
}
