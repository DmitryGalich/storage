#pragma once

#include <memory>

namespace storage
{
    namespace server
    {
        namespace network
        {
            struct Config
            {
                std::string host_{"127.0.0.1"};
                int port_{8080};
            };

            class NetworkModule
            {
            public:
                NetworkModule();
                ~NetworkModule();

                bool start(const Config &config);
                void stop();

            private:
                class NetworkModuleImpl;
                std::unique_ptr<NetworkModuleImpl> network_module_impl_;
            };
        } // namespace network

    } // namespace server

} // namespace storage
