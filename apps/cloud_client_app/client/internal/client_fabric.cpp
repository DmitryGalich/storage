#include "client_fabric.h"

#include "easylogging++.h"
#include "json.hpp"

#include "abstract_client.h"
#include "oatpp_client/oatpp_client.h"

namespace
{
    cloud::ClientConfig load_config(const std::string &config_path)
    {
        nlohmann::json json_object = {
            {"user_id", 123},
            {"user_name", "Dmitry"}};

        std::cout << json_object << std::endl;

        LOG(DEBUG) << config_path;
        return {};
    }
}

namespace cloud
{
    namespace internal
    {
        AbstractClient *create_client(const std::string &config_path)
        {
            auto config = load_config(config_path);
            return new OatppClient;
        }
    }
}
