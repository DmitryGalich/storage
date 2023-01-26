#include "client.h"

#include "easylogging++.h"

#include "internal/client_fabric.h"
#include "internal/abstract_client.h"

namespace cloud
{
    class Client::ClientImpl
    {
    public:
        ClientImpl() = default;
        ~ClientImpl() = default;

        void start();
        void stop() noexcept;

    private:
        std::unique_ptr<cloud::internal::AbstractClient> client_;
    };

    void Client::ClientImpl::start()
    {
        client_.reset(cloud::internal::create_client());
        if (!client_)
        {
            static const std::string kErrorText("Client not created");
            LOG(ERROR) << kErrorText;
            throw std::runtime_error(kErrorText);
        }

        client_->start();
        LOG(INFO) << "Started";
    }

    void Client::ClientImpl::stop() noexcept
    {
        if (!client_)
        {
            LOG(WARNING) << "Client already null";
            return;
        }

        try
        {
            client_->stop();
        }
        catch (const std::exception &e)
        {
            LOG(ERROR) << "Error while stopping client. " << e.what();
            return;
        }

        LOG(INFO) << "Stopped";
    }

    // Outside

    Client::Client() : client_impl_(std::make_unique<cloud::Client::ClientImpl>())
    {
    }

    Client::~Client()
    {
    }

    void Client::start()
    {
        client_impl_->start();
    }

    void Client::stop() noexcept
    {
        client_impl_->stop();
    }
}