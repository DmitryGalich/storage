#include "oatpp_client.h"

#include "easylogging++.h"

#include "oatpp/core/async/Executor.hpp"
#include "oatpp/core/base/Environment.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp/web/client/HttpRequestExecutor.hpp"
#include "oatpp/core/async/Executor.hpp"

#include "../abstract_client.h"

#include "api_client.hpp"
#include "async_executor.hpp"

namespace cloud
{
    namespace internal
    {
        class OatppClient::OatppClientImpl
        {
        public:
            OatppClientImpl();
            ~OatppClientImpl();

            bool start();
            void stop();

        private:
            std::shared_ptr<oatpp::parser::json::mapping::ObjectMapper> object_mapper_;
            std::shared_ptr<oatpp::network::tcp::client::ConnectionProvider> connection_provider_;
            std::shared_ptr<oatpp::web::client::HttpRequestExecutor> http_request_executor_;
            std::shared_ptr<ClientApiHolder> client_api_holder_;
            oatpp::async::Executor async_executor_;
        };

        OatppClient::OatppClientImpl::OatppClientImpl()
        {
            LOG(DEBUG) << "Constructor";
        }
        OatppClient::OatppClientImpl::~OatppClientImpl()
        {
            LOG(DEBUG) << "Destructor";
        }

        bool OatppClient::OatppClientImpl::start()
        {
            LOG(INFO) << "Starting...";

            oatpp::base::Environment::init();

            object_mapper_.reset();
            object_mapper_ = oatpp::parser::json::mapping::ObjectMapper::createShared();
            if (!object_mapper_)
            {
                LOG(ERROR) << "ObjectMapper is not created";
                return false;
            }

            connection_provider_.reset();
            connection_provider_ = oatpp::network::tcp::client::ConnectionProvider::createShared({"httpbin.org", 80});
            if (!connection_provider_)
            {
                LOG(ERROR) << "ConnectionProvider is not created";
                return false;
            }

            http_request_executor_.reset();
            http_request_executor_ = oatpp::web::client::HttpRequestExecutor::createShared(connection_provider_);
            if (!http_request_executor_)
            {
                LOG(ERROR) << "HttpRequestExecutor is not created";
                return false;
            }

            client_api_holder_.reset();
            client_api_holder_ = ClientApiHolder::createShared(http_request_executor_, object_mapper_);
            if (!http_request_executor_)
            {
                LOG(ERROR) << "ClientApiHolder is not created";
                return false;
            }

            LOG(INFO) << "Started";

            AsyncExecutorLol::runExample(client_api_holder_);

            return true;
        }

        void OatppClient::OatppClientImpl::stop()
        {
            LOG(INFO) << "Stopping...";

            async_executor_.waitTasksFinished();
            async_executor_.stop();
            async_executor_.join();

            object_mapper_.reset();
            connection_provider_.reset();
            http_request_executor_.reset();
            client_api_holder_.reset();

            oatpp::base::Environment::destroy();

            LOG(INFO) << "Stopped";
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

        bool OatppClient::start()
        {
            if (!client_impl_)
            {
                static const std::string kErrorText("Implementation is not created");
                LOG(ERROR) << kErrorText;
                throw std::runtime_error(kErrorText);
            }

            return client_impl_->start();
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