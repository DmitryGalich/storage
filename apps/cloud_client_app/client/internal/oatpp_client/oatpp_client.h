#define OATPP_CLIENT_H
#ifdef OATPP_CLIENT_H

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

#endif