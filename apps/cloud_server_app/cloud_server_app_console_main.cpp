#include "configs/cmake_config.h"

#include "easylogging++.h"

#include "server/server.h"

INITIALIZE_EASYLOGGINGPP

void configure_logger()
{
    el::Configurations conf(CMAKE_CURRENT_SOURCE_DIR + std::string{"/configs/log_config.conf"});
    el::Loggers::reconfigureAllLoggers(conf);
}

int main()
{
    configure_logger();

    LOG(INFO) << "================================";
    LOG(INFO) << PROJECT_NAME;
    LOG(INFO) << "version " << PROJECT_VERSION;

    cloud::Server server;
    try
    {
        if (!server.start(CMAKE_CURRENT_SOURCE_DIR + std::string{"/configs/server_config.json"}))
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
    server.stop();

    return 0;
}