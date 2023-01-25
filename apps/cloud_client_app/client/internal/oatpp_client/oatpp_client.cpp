#include "oatpp_client.h"

#include "easylogging++.h"

#include "oatpp/core/async/Executor.hpp"
#include "oatpp/core/base/Environment.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp/web/client/HttpRequestExecutor.hpp"

#include "../abstract_client.h"

#include "api_client.hpp"

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
            auto request_executor = oatpp::web::client::HttpRequestExecutor::createShared(connection_provider);
            auto client = DemoApiClient::createShared(request_executor, object_mapper);
            constexpr static const char *TAG = "SimpleExample";

            {
                auto data = client->doGet()->readBodyToString();
                OATPP_LOGD(TAG, "[doGet] data='%s'", data->c_str());
            }

            {
                auto data = client->doPost("Some data passed to POST")->readBodyToString();
                OATPP_LOGD(TAG, "[doPost] data='%s'", data->c_str());
            }

            {
                auto data = client->doGetAnything("path-parameter")->readBodyToString();
                OATPP_LOGD(TAG, "[doGetAnything] data='%s'", data->c_str());
            }

            {
                auto data = client->doPostAnything("path-parameter", "Some body here")->readBodyToString();
                OATPP_LOGD(TAG, "[doPostAnything] data='%s'", data->c_str());
            }

            {
                auto dto = RequestDto::createShared();
                dto->message = "Some message";
                dto->code = 200;
                auto data = client->doPostWithDto(dto)->readBodyToString();
                OATPP_LOGD(TAG, "[doPostWithDto] data='%s'", data->c_str());
            }

            LOG(INFO) << "OATPP Started";
        }
        void OatppClient::stop()
        {
            oatpp::base::Environment::destroy();

            LOG(INFO) << "OATPP Stopped";
        }
    }
}