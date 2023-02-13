#include "oatpp_client.h"

#include "easylogging++.h"

#include "oatpp/core/base/Environment.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp-websocket/Connector.hpp"
#include "oatpp-websocket/WebSocket.hpp"

#include "../abstract_client.h"
#include "websocket_listener.hpp"

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

            void run_socket_task();

        private:
            const ClientConfig kConfig_;

            std::shared_ptr<oatpp::network::tcp::client::ConnectionProvider> connection_provider_;
            std::shared_ptr<oatpp::websocket::Connector> socket_connector_;
            std::shared_ptr<oatpp::websocket::WebSocket> web_socket_;

            std::mutex web_socket_write_mtx_;
        };

        OatppClient::OatppClientImpl::OatppClientImpl(const ClientConfig &config)
            : kConfig_(config)
        {
        }

        bool OatppClient::OatppClientImpl::init()
        {
            oatpp::base::Environment::init();

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

            socket_connector_.reset();
            socket_connector_ = oatpp::websocket::Connector::createShared(connection_provider_);
            if (!socket_connector_)
            {
                LOG(ERROR) << "Connector is not created";
                return false;
            }

            auto connection = socket_connector_->connect("");

            web_socket_.reset();
            web_socket_ = oatpp::websocket::WebSocket::createShared(connection, true /* maskOutgoingMessages must be true for clients */);
            if (!web_socket_)
            {
                LOG(ERROR) << "WebSocket is not created";
                return false;
            }

            web_socket_->setListener(std::make_shared<WebSocketListener>(web_socket_write_mtx_));

            return true;
        }

        bool OatppClient::OatppClientImpl::run()
        {
            web_socket_->sendOneFrameText("hello");
            run_socket_task();

            return true;
        }

        void OatppClient::OatppClientImpl::run_socket_task()
        {
            web_socket_->listen();
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
            web_socket_.reset();
            socket_connector_.reset();
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