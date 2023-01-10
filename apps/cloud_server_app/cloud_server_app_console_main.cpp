#include "configs/cmake_config.h"

#include "easylogging++.h"

#include "server/server.h"

INITIALIZE_EASYLOGGINGPP

int main()
{
    const std::string kLogConfigPath = CMAKE_CURRENT_SOURCE_DIR + std::string{"/configs/log_config.conf"};

    el::Loggers::configureFromGlobal(kLogConfigPath.c_str());

    LOG(INFO) << PROJECT_NAME;
    LOG(INFO) << "version " << PROJECT_VERSION;

    cloud::Server server;
    server.start();
    server.stop();

    return 0;
}