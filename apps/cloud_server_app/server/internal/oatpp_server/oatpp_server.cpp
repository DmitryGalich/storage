#include "oatpp_server.h"

#include "easylogging++.h"

#include "oatpp/core/base/Environment.hpp"
#include "oatpp/core/async/Executor.hpp"
#include "oatpp/network/ConnectionProvider.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "../abstract_server.h"

namespace cloud
{
    namespace internal
    {
        class OatppServer::OatppServerImpl
        {
        public:
            OatppServerImpl() = delete;
            OatppServerImpl(const ServerConfig &config);
            ~OatppServerImpl() = default;

            bool start();
            void stop();

        private:
            bool init();
            bool run();

        private:
            const ServerConfig kConfig_;

            std::shared_ptr<oatpp::async::Executor> executor_;
            std::shared_ptr<oatpp::network::ServerConnectionProvider> connection_provider_;
            std::shared_ptr<oatpp::web::server::HttpRouter> router_;
            std::shared_ptr<oatpp::network::ConnectionHandler> connection_handler_;
            std::shared_ptr<oatpp::parser::json::mapping::Serializer::Config> serializer_config_;
            std::shared_ptr<oatpp::parser::json::mapping::Deserializer::Config> deserializer_config_;
            std::shared_ptr<oatpp::data::mapping::ObjectMapper> object_mapper_;
        };

        OatppServer::OatppServerImpl::OatppServerImpl(const ServerConfig &config) : kConfig_(config) {}

        bool OatppServer::OatppServerImpl::init()
        {
            oatpp::base::Environment::init();

            executor_.reset();
            executor_ = std::make_shared<oatpp::async::Executor>(
                9 /* Data-Processing threads */,
                2 /* I/O threads */,
                1 /* Timer threads */
            );
            if (!executor_)
            {
                LOG(ERROR) << "ConnectionProvider is not created";
                return false;
            }

            connection_provider_.reset();
            connection_provider_ = oatpp::network::tcp::server::ConnectionProvider::createShared(
                {kConfig_.host_,
                 static_cast<v_uint16>(kConfig_.port_),
                 (kConfig_.is_ip_v6_family_ ? oatpp::network::Address::IP_6 : oatpp::network::Address::IP_4)});
            if (!connection_provider_)
            {
                LOG(ERROR) << "ConnectionProvider is not created";
                return false;
            }

            router_.reset();
            router_ = oatpp::web::server::HttpRouter::createShared();
            if (!router_)
            {
                LOG(ERROR) << "HttpRouter is not created";
                return false;
            }

            connection_handler_.reset();
            connection_handler_ = oatpp::web::server::AsyncHttpConnectionHandler::createShared(router_, executor_);
            if (!connection_handler_)
            {
                LOG(ERROR) << "AsyncHttpConnectionHandler is not created";
                return false;
            }

            serializer_config_.reset();
            serializer_config_ = oatpp::parser::json::mapping::Serializer::Config::createShared();
            if (!serializer_config_)
            {
                LOG(ERROR) << "Serializer::Config is not created";
                return false;
            }

            deserializer_config_.reset();
            deserializer_config_ = oatpp::parser::json::mapping::Deserializer::Config::createShared();
            deserializer_config_->allowUnknownFields = false;
            if (!deserializer_config_)
            {
                LOG(ERROR) << "Deserializer::Config is not created";
                return false;
            }

            object_mapper_.reset();
            object_mapper_ = oatpp::parser::json::mapping::ObjectMapper::createShared(serializer_config_, deserializer_config_);
            if (!object_mapper_)
            {
                LOG(ERROR) << "ObjectMapper is not created";
                return false;
            }

            return true;
        }

        bool OatppServer::OatppServerImpl::run()
        {

            return true;
        }

        bool OatppServer::OatppServerImpl::start()
        {
            if (!init())
                return false;

            if (!run())
                return false;

            return true;
        }

        void OatppServer::OatppServerImpl::stop()
        {
            executor_.reset();
            connection_provider_.reset();
            router_.reset();
            connection_handler_.reset();
            serializer_config_.reset();
            deserializer_config_.reset();
            object_mapper_.reset();

            oatpp::base::Environment::destroy();
        }
    }
}

namespace cloud
{
    namespace internal
    {
        OatppServer::OatppServer(const ServerConfig &config) : server_impl_(std::make_unique<OatppServer::OatppServerImpl>(config))
        {
        }
        OatppServer::~OatppServer()
        {
        }

        bool OatppServer::start()
        {
            if (!server_impl_)
            {
                static const std::string kErrorText("Implementation is not created");
                LOG(ERROR) << kErrorText;
                throw std::runtime_error(kErrorText);
            }

            return server_impl_->start();
        }
        void OatppServer::stop()
        {
            if (!server_impl_)
            {
                LOG(ERROR) << "Implementation is not created";
                return;
            }

            server_impl_->stop();
        }
    }
}