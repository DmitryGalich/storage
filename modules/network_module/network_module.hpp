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

                std::map<Url, HttpCallback> http_callbacks_;

                web_sockets::InputCallback input_callback_;
                web_sockets::OutputCallback output_callback_;
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

                web_sockets::InputCallback input_callback_;
                web_sockets::OutputClientCallback output_callback_;
            };

        public:
            Client();
            ~Client();

            bool start(const Config &config);
            void stop();

        private:
            class ClientImpl;
            std::unique_ptr<ClientImpl> client_impl_;
        };
    }
}
