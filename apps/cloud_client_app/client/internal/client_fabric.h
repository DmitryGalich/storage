#define CLIENT_FABRIC_H
#ifdef CLIENT_FABRIC_H

#include "abstract_client.h"

namespace cloud
{
    namespace internal
    {
        AbstractClient *create_client();
    }
}

#endif