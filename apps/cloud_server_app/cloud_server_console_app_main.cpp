#include <iostream>

#include "cmake_config.h"

#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

int main()
{
    LOG(INFO) << cloud_server_console_app_PROJECT_NAME;
    LOG(INFO) << "version " << cloud_server_console_app_VERSION_MAJOR << "." << cloud_server_console_app_VERSION_PATCH << "." << cloud_server_console_app_VERSION_PATCH;

    return 0;
}