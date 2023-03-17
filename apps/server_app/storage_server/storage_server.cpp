#include "storage_server.hpp"

#include "easylogging++.h"

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
    };

    Server::ServerImpl::ServerImpl()
    {
    }

    bool Server::ServerImpl::start(const std::string &config_path)
    {
        LOG(INFO) << "Starting...";

        return true;
    }

    void Server::ServerImpl::stop() noexcept
    {
        LOG(INFO) << "Stopping...";

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