#pragma once

namespace cloud
{
    namespace internal
    {
        class AbstractClient
        {
        public:
            virtual ~AbstractClient() = 0;

            virtual void start() = 0;
            virtual void stop() = 0;
        };
    }
}
