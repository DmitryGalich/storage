#include "server.hpp"

#include "easylogging++.h"

#include "oatpp/core/base/Environment.hpp"

#include "config_handling/config_handling.hpp"
#include "network_components.hpp"

namespace server
{
    bool kek()
    {
        server::NetworkComponents network_components;

        return true;
    }

    bool run(const std::string &config_path)
    {
        const auto kConfig = server::config::load_config(config_path);

        bool is_need_server_running(true);
        while (is_need_server_running)
        {
            oatpp::base::Environment::init();

            kek();
            is_need_server_running = false;

            oatpp::base::Environment::destroy();
        }

        return true;
    }
}
