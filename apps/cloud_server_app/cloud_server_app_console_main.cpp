#include "configs/cmake_config.h"

#include "easylogging++.h"

#include "server/server.hpp"

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

    try
    {
        if (!server::run(CMAKE_CURRENT_SOURCE_DIR + std::string{"/configs/server_config.json"}))
        {
            LOG(ERROR) << "Server stopped with error";
            return -1;
        }
    }
    catch (const std::exception &e)
    {
        LOG(ERROR) << "Server stopped with error: " << e.what();
        return -1;
    }

    LOG(INFO) << "Server stopped with success";
    return 0;
}