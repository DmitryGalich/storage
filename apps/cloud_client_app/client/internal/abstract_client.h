#pragma once

#include <string>

#include <functional>

namespace cloud
{
    struct ClientConfig
    {
        std::string network_lib_;
        std::string host_;
        int port_;
        bool is_ip_v6_family_;
    };

    namespace internal
    {
        class AbstractClient
        {
        public:
            virtual ~AbstractClient() {}

            virtual bool start() = 0;
            virtual void stop() = 0;
        };
    }
}
