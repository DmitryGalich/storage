#include "storage_client.hpp"

#include <fstream>

#include "easylogging++.h"
#include "json.hpp"

// #include "network/network_module.hpp"

namespace
{
    storage::client::network::Config load_config(const std::string &config_path)
    {
        nlohmann::json json_object;
        json_object["host"] = "127.0.0.1";
        json_object["port"] = 8080;

        std::fstream file(config_path);
        if (!file.is_open())
        {
            LOG(INFO) << "Creeating default config...";

            std::ofstream default_config_file(config_path);
            if (!default_config_file.is_open())
            {
                const std::string kErrorText{"Can't save default config to \"" + config_path + "\""};
                LOG(ERROR) << kErrorText;
                throw std::runtime_error(kErrorText);
            }

            default_config_file << json_object.dump(4);
            default_config_file.close();

            LOG(INFO) << "Created";
        }
        else
        {
            json_object = nlohmann::json::parse(file);
            file.close();
        }

        LOG(INFO) << "Current config: \n"
                  << json_object.dump(4);

        storage::client::network::Config config;
        json_object.at("host").get_to(config.host_);
        json_object.at("port").get_to(config.port_);

        return config;
    }
}

namespace storage
{
    class Client::ClientImpl
    {
    public:
        ClientImpl() = default;
        ~ClientImpl() = default;

        bool start(const std::string &config_path);
        void stop() noexcept;

    private:
        // std::unique_ptr<client::network::NetworkModule> netowrk_module_;
    };

    bool Client::ClientImpl::start(const std::string &config_path)
    {
        LOG(INFO) << "Starting...";

        // netowrk_module_.reset(new client::network::NetworkModule(kAvailableProcessorsCores_));
        // if (!netowrk_module_)
        //     return false;

        // if (!netowrk_module_->start(load_config(config_path)))
        //     return false;

        return true;
    }

    void Client::ClientImpl::stop() noexcept
    {
        LOG(INFO) << "Stopping...";

        // if (netowrk_module_)
        // {
        //     netowrk_module_->stop();
        // }
        // netowrk_module_.reset();

        LOG(INFO) << "Stopped";
    }
}

namespace storage
{
    Client::Client()
        : client_impl_(std::make_unique<storage::Client::ClientImpl>())
    {
    }

    Client::~Client() {}

    bool Client::start(const std::string &config_path)
    {
        if (!client_impl_)
        {
            static const std::string kErrorText("Implementation is not created");
            LOG(ERROR) << kErrorText;
            throw std::runtime_error(kErrorText);
        }

        return client_impl_->start(config_path);
    }

    void Client::stop() noexcept
    {
        if (!client_impl_)
        {
            LOG(ERROR) << "Implementation is not created";
            return;
        }

        client_impl_->stop();
    }
}
