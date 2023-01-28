#pragma once

#include <string>

namespace cloud
{
    struct ClientConfig
    {
        std::string network_lib_;
        std::string host_;
        int port_;
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
