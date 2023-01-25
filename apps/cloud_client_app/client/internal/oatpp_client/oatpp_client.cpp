#include "oatpp_client.h"

#include "easylogging++.h"

#include "oatpp/core/base/Environment.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"

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
            oatpp::base::Environment::init();

            auto object_mapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
            auto connection_provider = oatpp::network::tcp::client::ConnectionProvider::createShared({"httpbin.org", 80});

            LOG(INFO) << "OATPP Started";
        }
        void OatppClient::stop()
        {
            oatpp::base::Environment::destroy();

            LOG(INFO) << "OATPP Stopped";
        }
    }
}