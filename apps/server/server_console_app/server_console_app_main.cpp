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

// namespace
// {
//     std::function<void(int)> shutdown_handler;
//     void handle_interruption(int signal)
//     {
//         shutdown_handler(signal);
//     }
// }

int main()
{
    configure_logger();

    LOG(INFO) << "================================";
    LOG(INFO) << PROJECT_NAME;
    LOG(INFO) << "version " << PROJECT_VERSION;

    const auto kProcessorsCores = std::thread::hardware_concurrency();
    const auto kServerWorkersNumber = (kProcessorsCores > 1) ? (kProcessorsCores - 1) : 1;

    storage::server::Server server;

    // // shutdown_handler = [&](int)
    // // {
    // //     server.stop();
    // // };

    // // struct sigaction signal_handler;
    // // signal_handler.sa_handler = handle_interruption;
    // // sigemptyset(&signal_handler.sa_mask);
    // // signal_handler.sa_flags = 0;
    // // sigaction(SIGINT, &signal_handler, NULL);

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

        if (input == "q")
        {
            server.stop();
            break;
        }
    }

    return 0;
}
