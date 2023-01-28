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

namespace
{
    class SendDtoCoroutine : public oatpp::async::Coroutine<SendDtoCoroutine>
    {

    public:
        SendDtoCoroutine(const std::shared_ptr<ClientApiHolder> client,
                         const oatpp::String &message,
                         v_int32 code)
            : client_(client), message_(message), code_(code)
        {
        }

        /**
         * Create DTO and send it via doPostWithDtoAsync method
         */
        Action act() override
        {
            auto dto = RequestDto::createShared();
            dto->message = message_;
            dto->code = code_;
            return client_->doPostWithDtoAsync(dto).callbackTo(&SendDtoCoroutine::onResponse);
        }

        /**
         * Receive response and read body asynchronously
         */
        Action onResponse(const std::shared_ptr<oatpp::web::protocol::http::incoming::Response> &response)
        {
            return response->readBodyToStringAsync().callbackTo(&SendDtoCoroutine::onBody);
        }

        /**
         * Print read body from the stream and finish
         */
        Action onBody(const oatpp::String &body)
        {
            OATPP_LOGD("[doPostWithDtoAsync] data='%s'", body->c_str());
            return finish();
        }

    private:
        std::shared_ptr<ClientApiHolder> client_;
        oatpp::String message_;
        v_int32 code_;
    };
}

namespace cloud
{
    namespace internal
    {
        class OatppClient::OatppClientImpl
        {
        public:
            OatppClientImpl() = delete;
            OatppClientImpl(const ClientConfig &config);
            ~OatppClientImpl() = default;

            bool start();
            void stop();

        private:
            bool init();
            bool run();

        private:
            const ClientConfig kConfig_;

            std::shared_ptr<oatpp::parser::json::mapping::ObjectMapper> object_mapper_;
            std::shared_ptr<oatpp::network::tcp::client::ConnectionProvider> connection_provider_;
            std::shared_ptr<oatpp::web::client::HttpRequestExecutor> http_request_executor_;
            std::shared_ptr<ClientApiHolder> client_api_holder_;
            oatpp::async::Executor async_executor_;
        };

        OatppClient::OatppClientImpl::OatppClientImpl(const ClientConfig &config) : kConfig_(config) {}

        bool OatppClient::OatppClientImpl::init()
        {
            oatpp::base::Environment::init();

            object_mapper_.reset();
            object_mapper_ = oatpp::parser::json::mapping::ObjectMapper::createShared();
            if (!object_mapper_)
            {
                LOG(ERROR) << "ObjectMapper is not created";
                return false;
            }

            connection_provider_.reset();
            connection_provider_ = oatpp::network::tcp::client::ConnectionProvider::createShared({kConfig_.host_, static_cast<v_uint16>(kConfig_.port_)});
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

            return true;
        }

        bool OatppClient::OatppClientImpl::run()
        {
            async_executor_.execute<SendDtoCoroutine>(client_api_holder_, "message1", 10000);

            return true;
        }

        bool OatppClient::OatppClientImpl::start()
        {
            if (!init())
                return false;

            if (!run())
                return false;

            return true;
        }

        void OatppClient::OatppClientImpl::stop()
        {

            async_executor_.waitTasksFinished();
            async_executor_.stop();
            async_executor_.join();

            object_mapper_.reset();
            connection_provider_.reset();
            http_request_executor_.reset();
            client_api_holder_.reset();

            oatpp::base::Environment::destroy();
        }
    }
}

namespace cloud
{
    namespace internal
    {
        OatppClient::OatppClient(const ClientConfig &config) : client_impl_(std::make_unique<OatppClient::OatppClientImpl>(config))
        {
        }
        OatppClient::~OatppClient()
        {
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