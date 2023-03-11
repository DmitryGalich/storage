#include "oatpp_server.h"

#include "easylogging++.h"

#include "oatpp/core/base/Environment.hpp"
#include "oatpp/network/Server.hpp"

#include "AppComponent.hpp"
#include "./controller/MyController.hpp"

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
        };

        OatppServer::OatppServerImpl::OatppServerImpl(const ServerConfig &config) : kConfig_(config) {}

        bool OatppServer::OatppServerImpl::init()
        {
            oatpp::base::Environment::init();

            /* Register Components in scope of run() method */
            AppComponent components;

            /* Get router component */
            OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

            /* Create MyController and add all of its endpoints to router */
            router->addController(std::make_shared<MyController>());

            /* Get connection handler component */
            OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler, "http");

            /* Get connection provider component */
            OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

            /* Create server which takes provided TCP connections and passes them to HTTP connection handler */
            oatpp::network::Server server(connectionProvider, connectionHandler);

            /* Run server */
            server.run();

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