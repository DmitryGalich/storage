#include "oatpp_server.h"

#include "easylogging++.h"

#include "oatpp/core/base/Environment.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

#include "server_api_controller.hpp"

#include "websocket/WSListener.hpp"

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
            const ServerConfig kConfig_;
            std::shared_ptr<oatpp::network::Server> server_;
        };

        OatppServer::OatppServerImpl::OatppServerImpl(const ServerConfig &config) : kConfig_(config) {}

        bool OatppServer::OatppServerImpl::start()
        {
            oatpp::base::Environment::init();

            OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor)
            ([&]
             { return std::make_shared<oatpp::async::Executor>(
                   kConfig_.executor_data_processing_threads_,
                   kConfig_.executor_io_threads_,
                   kConfig_.executor_timer_threads_); }());

            OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)
            ([&]
             { return oatpp::network::tcp::server::ConnectionProvider::createShared(
                   {kConfig_.host_,
                    static_cast<v_uint16>(kConfig_.port_),
                    (kConfig_.is_ip_v6_family_ ? oatpp::network::Address::IP_6
                                               : oatpp::network::Address::IP_4)}); }());

            OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)
            ([]
             { return oatpp::web::server::HttpRouter::createShared(); }());

            OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)
            ("http", []
             {  OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);  
                OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor); 
                return oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, executor); }());

            OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper)
            ([]
             { return oatpp::parser::json::mapping::ObjectMapper::createShared(); }());

            OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, websocketConnectionHandler)
            ("websocket", []
             {
                OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);
                auto connection_handler = oatpp::websocket::AsyncConnectionHandler::createShared(executor);
                connection_handler->setSocketInstanceListener(std::make_shared<WSInstanceListener>());
                return connection_handler; }());

            OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
            router->addController(std::make_shared<ServerApiController>());

            OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connection_handler, "http");
            OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

            server_.reset();
            server_ = std::make_shared<oatpp::network::Server>(connectionProvider, connection_handler);
            server_->run();

            return true;
        }

        void OatppServer::OatppServerImpl::stop()
        {
            server_->stop();
            server_.reset();

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