#include "configs/cmake_config.h"

#include "easylogging++.h"

#include "client/client.h"

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

    cloud::Client cloud;
    cloud.start();
    cloud.stop();

    LOG(INFO) << "================================";

    return 0;
}