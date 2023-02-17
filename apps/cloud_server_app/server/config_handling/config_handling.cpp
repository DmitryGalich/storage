#include "config_handling.hpp"

#include "easylogging++.h"
#include "json.hpp"

struct ConfigJsonArgs
{
    const std::string kHost_{"host"};
    const std::string kPort_{"port"};
    const std::string kIsIpV6Family_{"is_ip_v6_family"};

    const std::string kExecutorDataProcessingThreads_{"executor_data_processing_threads"};
    const std::string kExecutorIOThreads_{"executor_io_threads"};
    const std::string kExecutorTimerThreads_{"executor_timer_threads"};

} const kJsonArgs_;

namespace server
{
    namespace config
    {
        Config load_config(const std::string &path)
        {
            LOG(INFO) << "Loading config: " << path;

            nlohmann::json json_object;
            json_object[kJsonArgs_.kHost_] = "127.0.0.1";
            json_object[kJsonArgs_.kPort_] = 8000;
            json_object[kJsonArgs_.kIsIpV6Family_] = false;
            json_object[kJsonArgs_.kExecutorDataProcessingThreads_] = 1;
            json_object[kJsonArgs_.kExecutorIOThreads_] = 1;
            json_object[kJsonArgs_.kExecutorTimerThreads_] = 1;

            std::fstream file(path);
            if (!file.is_open())
            {
                LOG(INFO) << "Creeating default config...";

                std::ofstream default_config_file(path);
                if (!default_config_file.is_open())
                {
                    const std::string kErrorText{"Can't save default config to \"" + path + "\""};
                    LOG(ERROR) << kErrorText;
                }
                else
                {
                    default_config_file << json_object.dump(4);
                    default_config_file.close();

                    LOG(INFO) << "Created";
                }
            }
            else
            {
                json_object = nlohmann::json::parse(file);
                file.close();
            }

            LOG(INFO) << "Current config: \n"
                      << json_object.dump(4);

            Config config;
            json_object.at(kJsonArgs_.kHost_).get_to(config.host_);
            json_object.at(kJsonArgs_.kPort_).get_to(config.port_);
            json_object.at(kJsonArgs_.kIsIpV6Family_).get_to(config.is_ip_v6_family_);
            json_object.at(kJsonArgs_.kExecutorDataProcessingThreads_).get_to(config.executor_data_processing_threads_);
            json_object.at(kJsonArgs_.kExecutorIOThreads_).get_to(config.executor_io_threads_);
            json_object.at(kJsonArgs_.kExecutorTimerThreads_).get_to(config.executor_timer_threads_);

            return config;
        }
    }
}