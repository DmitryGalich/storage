#include "storage_server.hpp"

#include "easylogging++.h"

// #include "network/network_module.hpp"

namespace storage
{
    class Server::ServerImpl
    {
    public:
        ServerImpl() = delete;
        ServerImpl(const int available_processors_cores);
        ~ServerImpl() = default;

        bool start(const std::string &config_path);
        void stop() noexcept;

    private:
        const int kAvailableProcessorsCores_;
    };

    Server::ServerImpl::ServerImpl(const int available_processors_cores)
        : kAvailableProcessorsCores_(available_processors_cores)
    {
    }

    bool Server::ServerImpl::start(const std::string &config_path)
    {
        LOG(INFO) << "Starting...";

        // netowrk_module_.reset(new client::network::NetworkModule(kAvailableProcessorsCores_));
        // if (!netowrk_module_)
        //     return false;

        // if (!netowrk_module_->start(load_config(config_path)))
        //     return false;

        return true;
    }

    void Server::ServerImpl::stop() noexcept
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
    Server::Server(const int available_processors_cores) : server_impl_(std::make_unique<storage::Server::ServerImpl>(available_processors_cores))
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