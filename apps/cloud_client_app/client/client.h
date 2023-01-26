#pragma once

#include <memory>

namespace cloud
{
    class Client
    {
    public:
        Client();
        ~Client();

        void start();
        void stop() noexcept;

    private:
        class ClientImpl;
        std::unique_ptr<cloud::Client::ClientImpl> client_impl_;
    };
}
