#pragma once

#include "../abstract_client.h"

namespace cloud
{
    namespace internal
    {
        class OatppClient : public AbstractClient
        {
        public:
            OatppClient();
            ~OatppClient() override;

            void start() override;
            void stop() override;
        };
    }
}
