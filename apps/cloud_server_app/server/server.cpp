#include "server.h"

#include "easylogging++.h"

#include "internal/server_fabric.h"
#include "internal/abstract_server.h"

namespace cloud
{
    class Server::ServerImpl
    {
    public:
        ServerImpl() = default;
        ~ServerImpl() = default;

        bool start(const std::string &config_path);
        void stop() noexcept;

    private:
        std::unique_ptr<cloud::internal::AbstractServer> server_;
    };

    bool Server::ServerImpl::start(const std::string &config_path)
    {
        LOG(INFO) << "Starting...";

        server_.reset(cloud::internal::create_server(config_path));
        if (!server_)
        {
            static const std::string kErrorText("Server not created");
            LOG(ERROR) << kErrorText;
            throw std::runtime_error(kErrorText);
        }

        return server_->start();
    }

    void Server::ServerImpl::stop() noexcept
    {
        LOG(INFO) << "Stopping...";

        if (!server_)
        {
            LOG(WARNING) << "Server already null";
            return;
        }

        try
        {
            server_->stop();
            server_.reset();
        }
        catch (const std::exception &e)
        {
            LOG(ERROR) << "Error while stopping server. " << e.what();
            return;
        }

        LOG(INFO) << "Stopped";
    }
}

namespace cloud
{
    Server::Server() : server_impl_(std::make_unique<Server::ServerImpl>())
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