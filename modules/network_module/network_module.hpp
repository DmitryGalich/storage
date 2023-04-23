#pragma once

#include <memory>
#include <string>
#include <utility>
#include <functional>
#include <map>

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

                struct Http
                {
                public:
                    typedef std::string Url;
                    typedef std::function<std::string()> Callback;

                public:
                    const std::pair<Url, Callback> get404() const;
                    void set404(const Url &url, const Callback &callback);

                    const std::map<Url, Callback> &getCallbacks() const;
                    void setCallback(const Url &url, const Callback &callback);

                private:
                    std::pair<Url, Callback> status_404_;
                    std::map<Url, Callback> callbacks_;
                };
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