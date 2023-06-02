#include <thread>
#include <future>

#include "configs/cmake_config.h"

#include "storage_server/storage_server.hpp"

#include "easylogging++.h"
#define ELPP_THREAD_SAFE

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
            return;
        }
    }
}

int main()
{
    try
    {
        configure_logger();

        LOG(INFO) << "================================";
        LOG(INFO) << PROJECT_NAME;
        LOG(INFO) << "version " << PROJECT_VERSION;

        std::promise<void> server_promise;
        auto server_future = server_promise.get_future();

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

        std::future<void> user_command_future = std::async(&wait_for_user_command);
        while (true)
        {
            if (user_command_future.wait_for(std::chrono::seconds(1)) == std::future_status::ready)
            {
                LOG(INFO) << "Signal to stop from user command";
                break;
            }

            if (server_future.wait_for(std::chrono::seconds(1)) == std::future_status::ready)
            {
                LOG(INFO) << "Signal to stop from client command";
                break;
            }
        }

        server.stop();
    }
    catch (const std::exception &e)
    {
        LOG(ERROR) << "Global error" << e.what();
    }

    return 0;
}
