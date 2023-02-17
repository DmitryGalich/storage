#include "server.hpp"

#include "easylogging++.h"

#include "oatpp/core/base/Environment.hpp"

#include "config_handling/config_handling.hpp"

namespace server
{
    bool run(const std::string &config_path)
    {
        LOG(INFO) << config_path;

        const auto kConfig = server::config::load_config(config_path);

        bool is_need_server_running(true);

        while (is_need_server_running)
        {
            oatpp::base::Environment::init();

            is_need_server_running = false;

            oatpp::base::Environment::destroy();
        }

        return true;
    }
}
