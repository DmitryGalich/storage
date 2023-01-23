#include "client.h"

#include "easylogging++.h"

#include "oatpp/core/base/Environment.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/web/client/HttpRequestExecutor.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"

#include "DemoApiClient.hpp"

#include "AsyncExample.hpp"
#include "SimpleExample.hpp"

namespace cloud
{
    Client::Client()
    {
        LOG(DEBUG) << "Constructor";
    }

    Client::~Client()
    {
        LOG(DEBUG) << "Destructor";
    }

    void Client::start()
    {
        oatpp::base::Environment::init();
        auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
        auto connectionProvider = oatpp::network::tcp::client::ConnectionProvider::createShared({"httpbin.org", 80});
        auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(connectionProvider);
        auto client = DemoApiClient::createShared(requestExecutor, objectMapper);

        LOG(INFO) << "Started";

        SimpleExample::runExample(client);
    }

    void Client::stop()
    {
        oatpp::base::Environment::destroy();

        LOG(INFO) << "Stopped";
    }
}