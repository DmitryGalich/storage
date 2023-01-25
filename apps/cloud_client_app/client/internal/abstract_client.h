#pragma once

namespace cloud
{
    namespace internal
    {
        class AbstractClient
        {
        public:
            virtual ~AbstractClient() {}

            virtual void start() = 0;
            virtual void stop() = 0;
        };
    }
}
