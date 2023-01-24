#include "oatpp_client.h"

#include "easylogging++.h"

#include "../abstract_client.h"

namespace cloud
{
    namespace internal
    {
        OatppClient::OatppClient()
        {
            LOG(DEBUG) << "Constructor";
        }
        OatppClient::~OatppClient()
        {
            LOG(DEBUG) << "Destructor";
        }

        void OatppClient::start()
        {
            LOG(INFO) << "OATPP Started";
        }
        void OatppClient::stop()
        {
            LOG(INFO) << "OATPP Stopped";
        }
    }
}