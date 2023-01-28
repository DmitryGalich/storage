#pragma once

#include <string>

namespace cloud
{
    struct ClientConfig
    {
        std::string host_{"127.0.0.1"};
        int port_{80};
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
