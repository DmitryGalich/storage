#pragma once

#include <memory>

namespace cloud
{
    class Client
    {
    public:
        struct Config
        {
            std::string host_;
            int port_;
        };

    public:
        Client();
        Client(const std::string &config_path);
        ~Client();

        bool start();
        void stop() noexcept;

    private:
        class ClientImpl;
        std::unique_ptr<cloud::Client::ClientImpl> client_impl_;
    };
}
