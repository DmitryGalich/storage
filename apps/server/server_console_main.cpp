
#include "oatpp/core/base/Environment.hpp"

#include "network_components.hpp"

int main()
{

    oatpp::base::Environment::init();

    NetworkComponents components;

    // try
    // {
    //     if (!server::run(CMAKE_CURRENT_SOURCE_DIR + std::string{"/configs/server_config.json"}))
    //     {
    //         LOG(ERROR) << "Server stopped with error";
    //         return -1;
    //     }
    // }
    // catch (const std::exception &e)
    // {
    //     LOG(ERROR) << "Server stopped with error: " << e.what();
    //     return -1;
    // }

    oatpp::base::Environment::destroy();

    return 0;
}