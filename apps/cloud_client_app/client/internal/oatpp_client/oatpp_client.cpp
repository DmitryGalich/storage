#include "oatpp_client.h"

#include "easylogging++.h"

#include "oatpp/core/async/Executor.hpp"
#include "oatpp/core/base/Environment.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp/web/client/HttpRequestExecutor.hpp"

#include "../abstract_client.h"

#include "api_client.hpp"
#include "async_handler.hpp"

namespace cloud
{
    namespace internal
    {
        class OatppClient::OatppClientImpl
        {
        public:
            OatppClientImpl();
            ~OatppClientImpl();

            void start();
            void stop();

        private:
            std::shared_ptr<oatpp::parser::json::mapping::ObjectMapper> object_mapper_;
            std::shared_ptr<oatpp::network::tcp::client::ConnectionProvider> connection_provider_;
            std::shared_ptr<oatpp::web::client::HttpRequestExecutor> http_request_executor_;
            std::shared_ptr<DemoApiClient> client_;
        };

        OatppClient::OatppClientImpl::OatppClientImpl()
        {
            LOG(DEBUG) << "Constructor";
        }
        OatppClient::OatppClientImpl::~OatppClientImpl()
        {
            LOG(DEBUG) << "Destructor";
        }

        void OatppClient::OatppClientImpl::start()
        {
            oatpp::base::Environment::init();

            object_mapper_.reset();
            object_mapper_ = oatpp::parser::json::mapping::ObjectMapper::createShared();

            connection_provider_.reset();
            connection_provider_ = oatpp::network::tcp::client::ConnectionProvider::createShared({"httpbin.org", 80});

            http_request_executor_.reset();
            http_request_executor_ = oatpp::web::client::HttpRequestExecutor::createShared(connection_provider_);

            client_.reset();
            client_ = DemoApiClient::createShared(http_request_executor_, object_mapper_);

            LOG(INFO) << "OATPP Started";

            AsyncExample::runExample(client_);
        }

        void OatppClient::OatppClientImpl::stop()
        {
            oatpp::base::Environment::destroy();
            LOG(INFO) << "OATPP Stopped";
        }
    }
}

namespace cloud
{
    namespace internal
    {
        OatppClient::OatppClient() : client_impl_(std::make_unique<OatppClient::OatppClientImpl>())
        {
            LOG(DEBUG) << "Constructor";
        }
        OatppClient::~OatppClient()
        {
            LOG(DEBUG) << "Destructor";
        }

        void OatppClient::start()
        {
            if (!client_impl_)
            {
                static const std::string kErrorText("Implementation is not created");
                LOG(ERROR) << kErrorText;
                throw std::runtime_error(kErrorText);
            }

            client_impl_->start();
        }
        void OatppClient::stop()
        {
            if (!client_impl_)
            {
                LOG(ERROR) << "Implementation is not created";
                return;
            }

            client_impl_->stop();
        }
    }
}