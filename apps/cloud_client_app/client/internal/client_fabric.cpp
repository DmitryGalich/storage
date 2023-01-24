#include "client_fabric.h"

#include "easylogging++.h"

namespace cloud
{
    namespace internal
    {
        AbstractClient *create_client()
        {
            LOG(INFO) << "Creating client";
            return nullptr;
        }
    }
}
