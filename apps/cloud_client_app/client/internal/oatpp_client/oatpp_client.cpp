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
        };

        OatppClient::OatppClientImpl::OatppClientImpl(const ClientConfig &config)
            : kConfig_(config)

        {
        }

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
            connection_provider_ = oatpp::network::tcp::client::ConnectionProvider::createShared(
                {kConfig_.host_,
                 static_cast<v_uint16>(kConfig_.port_),
                 (kConfig_.is_ip_v6_family_ ? oatpp::network::Address::IP_6 : oatpp::network::Address::IP_4)});
            if (!connection_provider_)
            {
                LOG(ERROR) << "ConnectionProvider is not created";
                return false;
            }

            return true;
        }

        bool OatppClient::OatppClientImpl::run()
        {

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

            object_mapper_.reset();
            connection_provider_.reset();

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