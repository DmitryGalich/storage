#include "client_fabric.h"

#include <filesystem>
#include <fstream>

#include "easylogging++.h"
#include "json.hpp"

#include "abstract_client.h"
#include "oatpp_client/oatpp_client.h"

namespace
{
    struct ConfigSupport
    {
        struct ConfigJsonArgs
        {
            const std::string kHost_{"host"};
            const std::string kPort_{"port"};
            const std::string kNetworkLib_{"network_lib"};

        } const kJsonArgs_;

        struct NetworkLibsTitles
        {
            const std::string kOatpp_{"oatpp"};

        } const kNetworkLibsTitles_;

    } const kConfigSupport;
}

namespace
{
    cloud::ClientConfig load_config(const std::string &config_path)
    {
        std::fstream file(config_path);
        if (!file.is_open())
        {
            nlohmann::json defaut_config_json_object;
            defaut_config_json_object[kConfigSupport.kJsonArgs_.kNetworkLib_] = kConfigSupport.kNetworkLibsTitles_.kOatpp_;
            defaut_config_json_object[kConfigSupport.kJsonArgs_.kHost_] = "127.0.0.1";
            defaut_config_json_object[kConfigSupport.kJsonArgs_.kPort_] = 80;

            std::ofstream default_config_file(config_path);
            if (!default_config_file.is_open())
            {
                const std::string kErrorText{"Can't save default config to \"" + config_path + "\""};
                LOG(ERROR) << kErrorText;
                throw std::runtime_error(kErrorText);
            }

            default_config_file << defaut_config_json_object.dump(4);
            default_config_file.close();

            return {};
        }

        nlohmann::json json_object = nlohmann::json::parse(file);

        file.close();

        cloud::ClientConfig config;
        json_object.at(kConfigSupport.kJsonArgs_.kNetworkLib_).get_to(config.network_lib_);
        json_object.at(kConfigSupport.kJsonArgs_.kHost_).get_to(config.host_);
        json_object.at(kConfigSupport.kJsonArgs_.kPort_).get_to(config.port_);

        return config;
    }
}

namespace cloud
{
    namespace internal
    {
        AbstractClient *create_client(const std::string &config_path)
        {
            const auto config = load_config(config_path);

            if (config.network_lib_ == kConfigSupport.kNetworkLibsTitles_.kOatpp_)
                return new OatppClient(config);
            else
            {
                LOG(ERROR) << "Unknown title of network lib: \"" << config.network_lib_ << "\"";
                return nullptr;
            }
        }
    }
}
