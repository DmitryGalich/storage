#pragma once

#include "../abstract_client.h"

#include <memory>

namespace cloud
{
    namespace internal
    {
        class OatppClient : public AbstractClient
        {
        public:
            OatppClient();
            ~OatppClient() override;

            bool start() override;
            void stop() override;

        private:
            class OatppClientImpl;
            std::unique_ptr<OatppClientImpl> client_impl_;
        };
    }
}
