#include "../network_module.hpp"

#include "easylogging++.h"

namespace network_module
{
    class Server::ServerImpl
    {
    public:
        ServerImpl(const int &available_processors_cores);
        ~ServerImpl();

        bool start(const Server::Config &config);
        void stop();
    };

    Server::ServerImpl::ServerImpl(const int &available_processors_cores)
    {
        LOG(INFO) << "KEK";
    }

    Server::ServerImpl::~ServerImpl() {}

    bool Server::ServerImpl::start(const Server::Config &config)
    {
        return true;
    }

    void Server::ServerImpl::stop() {}
}

namespace network_module
{
    Server::Server(const int &available_processors_cores)
        : server_impl_(std::make_unique<ServerImpl>(available_processors_cores)) {}

    Server::~Server() {}

    bool Server::start(const Config &config)
    {
        LOG(INFO) << "Starting...";

        if (!server_impl_)
        {
            static const std::string kErrorText("Implementation is not created");
            LOG(ERROR) << kErrorText;
            throw std::runtime_error(kErrorText);
        }

        return server_impl_->start(config);
    }

    void Server::stop()
    {
        LOG(INFO) << "Stopping...";

        if (!server_impl_)
        {
            LOG(ERROR) << "Implementation is not created";
            return;
        }

        server_impl_->stop();

        LOG(INFO) << "Stopped";
    }
}
