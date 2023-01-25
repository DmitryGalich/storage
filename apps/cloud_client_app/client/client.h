#pragma once

#include <memory>

namespace cloud
{
    class ClientImpl;

    class Client
    {
    public:
        Client();
        ~Client();

        void start();
        void stop() noexcept;

    private:
        std::unique_ptr<ClientImpl> client_impl_;
    };
}
