#include "configs/cmake_config.h"

#include "easylogging++.h"

#include "client/client.h"

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

    cloud::Client cloud;
    try
    {
        cloud.start();
    }
    catch (const std::exception &e)
    {
        LOG(ERROR) << e.what();
        LOG(INFO) << "Shutting down the application";

        cloud.stop();
        return -1;
    }

    cloud.stop();

    return 0;
}