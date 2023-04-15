#pragma once

#include <memory>
#include <string>
#include <utility>
#include <functional>

namespace network_module
{
    namespace server
    {
        class Server
        {
        public:
            struct Config
            {
                static Config load_config(const std::string &config_path);

                std::string host_{"127.0.0.1"};
                int port_{8080};

                std::pair<std::string, std::function<void()>> http_callbacks_;
            };

        public:
            Server();
            ~Server();

            bool start(const int &available_processors_cores, const Config &config);
            void stop();

        private:
            class ServerImpl;
            std::unique_ptr<ServerImpl> server_impl_;
        };
    }

    // Client

    // class Client
    // {
    // public:
    //     struct Config
    //     {
    //         std::string host_{"127.0.0.1"};
    //         int port_{8080};
    //         bool is_ip_v6_{false};
    //     };

    // public:
    //     Client();
    //     ~Client();

    //     bool start(const Config &config);
    //     void stop();

    // private:
    //     class ClientImpl;
    //     std::unique_ptr<ClientImpl> client_impl_;
    // };
}