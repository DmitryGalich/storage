#pragma once

namespace cloud
{
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
