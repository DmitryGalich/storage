#include <iostream>

#include "cmake_config.h"

int main()
{
    std::cout << cloud_server_app_PROJECT_NAME << std::endl;
    std::cout << cloud_server_app_VERSION_MAJOR << std::endl;
    std::cout << cloud_server_app_VERSION_MINOR << std::endl;
    std::cout << cloud_server_app_VERSION_PATCH << std::endl;

    return 0;
}