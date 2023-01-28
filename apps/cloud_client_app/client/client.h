#pragma once

#include <memory>

namespace cloud
{
    class Client
    {
    public:
        struct Config
        {
            std::string host_{"127.0.0.1"};
            int port_{80};
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
