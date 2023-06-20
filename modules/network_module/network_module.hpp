#pragma once

#include <memory>
#include <string>
#include <utility>
#include <functional>
#include <map>

#include "network_module_common.hpp"

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

                struct Callbacks
                {
                    SignalToStop signal_to_stop_;

                    struct WebSocketsCallbacks
                    {
                        web_sockets::OnStartCallback process_new_connection_;
                        web_sockets::ReceivingCallback process_receiving_;
                    } web_sockets_callbacks_;

                    std::map<Url, HttpCallback> http_callbacks_;

                } callbacks_;
            };

        public:
            Server();
            ~Server();

            bool start(const int &available_processors_cores, const Config &config);
            void stop();

            bool send(const std::string &data);

        private:
            class ServerImpl;
            std::unique_ptr<ServerImpl> server_impl_;
        };
    }

    namespace client
    {
        class Client
        {
        public:
            struct Config
            {
                static Config load_config(const std::string &config_path);

                std::string host_{"127.0.0.1"};
                int port_{8080};

                int reconnect_timeout_sec_{5};
                int workers_number_{1};

                struct Callbacks
                {
                    SignalToStop signal_to_stop_;
                    web_sockets::OnStartCallback on_start_;
                    web_sockets::ReceivingCallback process_receiving_;
                } callbacks_;
            };

        public:
            Client();
            ~Client();

            bool start(const Config &config);
            void stop();

            bool is_running() const;

            bool send(const std::string &data);

        private:
            class ClientImpl;
            std::unique_ptr<ClientImpl> client_impl_;
        };
    }
}
