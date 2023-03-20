#include "storage_server.hpp"

#include <fstream>

#include "easylogging++.h"
#include "json.hpp"

#include "network/network_module.hpp"

namespace
{
    storage::server::network::Config load_config(const std::string &config_path)
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

        storage::server::network::Config config;
        json_object.at("host").get_to(config.host_);
        json_object.at("port").get_to(config.port_);

        return config;
    }
}

namespace storage
{
    class Server::ServerImpl
    {
    public:
        ServerImpl();
        ~ServerImpl() = default;

        bool start(const std::string &config_path);
        void stop() noexcept;

    private:
        std::unique_ptr<server::network::NetworkModule> netowrk_module_;
    };

    Server::ServerImpl::ServerImpl()
    {
    }

    bool Server::ServerImpl::start(const std::string &config_path)
    {
        LOG(INFO) << "Starting...";

        netowrk_module_.reset(new server::network::NetworkModule());
        if (!netowrk_module_)
            return false;

        if (!netowrk_module_->start(load_config(config_path)))
            return false;

        return true;
    }

    void Server::ServerImpl::stop() noexcept
    {
        LOG(INFO) << "Stopping...";

        if (netowrk_module_)
        {
            netowrk_module_->stop();
        }
        netowrk_module_.reset();

        LOG(INFO) << "Stopped";
    }
}

namespace storage
{
    Server::Server() : server_impl_(std::make_unique<storage::Server::ServerImpl>())
    {
    }

    Server::~Server()
    {
    }

    bool Server::start(const std::string &config_path)
    {
        if (!server_impl_)
        {
            static const std::string kErrorText("Implementation is not created");
            LOG(ERROR) << kErrorText;
            throw std::runtime_error(kErrorText);
        }

        return server_impl_->start(config_path);
    }

    void Server::stop() noexcept
    {
        if (!server_impl_)
        {
            LOG(ERROR) << "Implementation is not created";
            return;
        }

        server_impl_->stop();
    }

}