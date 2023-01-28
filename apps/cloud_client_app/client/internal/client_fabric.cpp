#include "client_fabric.h"

#include "easylogging++.h"

#include "abstract_client.h"
#include "oatpp_client/oatpp_client.h"

namespace cloud
{
    namespace internal
    {
        AbstractClient *create_client(const std::string &config_path)
        {
            LOG(DEBUG) << config_path;
            return new OatppClient;
        }
    }
}
