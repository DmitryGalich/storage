#include "configs/cmake_config.h"

#include "storage_client/storage_client.hpp"

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

    storage::Client client;

    // try
    // {
    //     if (!client.start(CMAKE_CURRENT_SOURCE_DIR + std::string{"/configs/server_config.json"}))
    //     {
    //         LOG(ERROR) << "Can't start client";
    //         LOG(INFO) << "Shutting down the application";
    //         client.stop();
    //         return -1;
    //     }
    // }
    // catch (const std::exception &e)
    // {
    //     LOG(ERROR) << e.what();
    //     LOG(INFO) << "Shutting down the application";
    //     client.stop();
    //     return -1;
    // }

    // client.stop();

    return 0;
}
