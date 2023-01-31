#pragma once

#include <string>

namespace cloud
{
    struct ServerConfig
    {
        std::string network_lib_;
        std::string host_;
        int port_;
        bool is_ip_v6_family_;
    };

    namespace internal
    {
        class AbstractServer
        {
        public:
            virtual ~AbstractServer() {}

            virtual bool start() = 0;
            virtual void stop() = 0;
        };
    }
}
