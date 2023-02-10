#include "oatpp_server.h"

#include "easylogging++.h"

#include "oatpp/core/base/Environment.hpp"
#include "oatpp/core/async/Executor.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp-websocket/AsyncConnectionHandler.hpp"
#include "oatpp-websocket/AsyncWebSocket.hpp"

#include "../abstract_server.h"

namespace
{
    /**
     * WebSocket listener listens on incoming WebSocket events.
     */
    class WSListener : public oatpp::websocket::AsyncWebSocket::Listener
    {

    private:
        /**
         * Buffer for messages. Needed for multi-frame messages.
         */
        oatpp::data::stream::BufferOutputStream m_messageBuffer;

    public:
        /**
         * Called on "ping" frame.
         */
        CoroutineStarter onPing(const std::shared_ptr<AsyncWebSocket> &socket, const oatpp::String &message) override
        {
            // OATPP_LOGD("Server_WSListener", "onPing");
            return socket->sendPongAsync(message);
        }

        /**
         * Called on "pong" frame
         */
        CoroutineStarter onPong(const std::shared_ptr<AsyncWebSocket> &socket, const oatpp::String &message) override
        {
            // OATPP_LOGD("Server_WSListener", "onPong");
            return nullptr; // do nothing
        }

        /**
         * Called on "close" frame
         */
        CoroutineStarter onClose(const std::shared_ptr<AsyncWebSocket> &socket, v_uint16 code, const oatpp::String &message) override
        {
            // OATPP_LOGD("Server_WSListener", "onClose code=%d", code);
            return nullptr; // do nothing
        }

        /**
         * Called on each message frame. After the last message will be called once-again with size == 0 to designate end of the message.
         */
        CoroutineStarter readMessage(const std::shared_ptr<AsyncWebSocket> &socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size) override
        {
            if (size == 0)
            { // message transfer finished

                auto wholeMessage = m_messageBuffer.toString();
                m_messageBuffer.setCurrentPosition(0);

                // OATPP_LOGD("Server_WSListener", "onMessage message='%s'", wholeMessage->c_str());

                /* Send message in reply */
                return socket->sendOneFrameTextAsync("Hello from oatpp!: " + wholeMessage);
            }
            else if (size > 0)
            { // message frame received
                m_messageBuffer.writeSimple(data, size);
            }

            return nullptr; // do nothing
        }
    };
}

namespace
{
    /**
     * Listener on new WebSocket connections.
     */
    class WSInstanceListener : public oatpp::websocket::AsyncConnectionHandler::SocketInstanceListener
    {
    public:
        /**
         * Counter for connected clients.
         */
        std::atomic<int> sockets_;

    public:
        /**
         *  Called when socket is created
         */
        void onAfterCreate_NonBlocking(const std::shared_ptr<WSListener::AsyncWebSocket> &socket, const std::shared_ptr<const ParameterMap> &params) override
        {
            sockets_++;
            OATPP_LOGD("Server_WSInstanceListener", "New Incoming Connection. Connection count=%d", sockets_.load());

            /* In this particular case we create one WSListener per each connection */
            /* Which may be redundant in many cases */
            socket->setListener(std::make_shared<WSListener>());
        }

        /**
         *  Called before socket instance is destroyed.
         */
        void onBeforeDestroy_NonBlocking(const std::shared_ptr<WSListener::AsyncWebSocket> &socket) override
        {
            sockets_--;
            OATPP_LOGD("Server_WSInstanceListener", "Connection closed. Connection count=%d", sockets_.load());
        }
    };

}

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

            std::shared_ptr<oatpp::async::Executor> async_executor_;
            std::shared_ptr<oatpp::network::tcp::server::ConnectionProvider> connection_provider_;
            std::shared_ptr<oatpp::web::server::HttpRouter> router_;
            std::shared_ptr<oatpp::network::ConnectionHandler> connection_handler_;
            std::shared_ptr<oatpp::data::mapping::ObjectMapper> object_mapper_;
            std::shared_ptr<oatpp::websocket::AsyncConnectionHandler> socket_connection_handler_;
            std::shared_ptr<oatpp::websocket::AsyncConnectionHandler::SocketInstanceListener> socket_listener_;
            std::shared_ptr<oatpp::network::Server> server_;
        };

        OatppServer::OatppServerImpl::OatppServerImpl(const ServerConfig &config) : kConfig_(config) {}

        bool OatppServer::OatppServerImpl::init()
        {
            oatpp::base::Environment::init();

            async_executor_.reset();
            async_executor_ = std::make_shared<oatpp::async::Executor>(
                kConfig_.executor_data_processing_threads_,
                kConfig_.executor_io_threads_,
                kConfig_.executor_timer_threads_);
            if (!async_executor_)
            {
                LOG(ERROR) << "Executor is not created";
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
            connection_handler_ = oatpp::web::server::AsyncHttpConnectionHandler::createShared(router_, async_executor_);
            if (!connection_handler_)
            {
                LOG(ERROR) << "AsyncHttpConnectionHandler is not created";
                return false;
            }

            object_mapper_.reset();
            object_mapper_ = oatpp::parser::json::mapping::ObjectMapper::createShared();
            if (!object_mapper_)
            {
                LOG(ERROR) << "ObjectMapper is not created";
                return false;
            }

            socket_connection_handler_.reset();
            socket_connection_handler_ = oatpp::websocket::AsyncConnectionHandler::createShared(async_executor_);
            if (!socket_connection_handler_)
            {
                LOG(ERROR) << "websocket::AsyncConnectionHandler is not created";
                return false;
            }

            socket_listener_.reset();
            socket_listener_ = std::make_shared<WSInstanceListener>();
            if (!socket_listener_)
            {
                LOG(ERROR) << "SocketInstanceListener is not created";
                return false;
            }

            socket_connection_handler_->setSocketInstanceListener(socket_listener_);

            server_.reset();
            server_ = oatpp::network::Server::createShared(connection_provider_, connection_handler_);
            if (!server_)
            {
                LOG(ERROR) << "Server is not created";
                return false;
            }

            return true;
        }

        bool OatppServer::OatppServerImpl::run()
        {
            server_->run();

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
            if (server_)
                server_->stop();

            server_.reset();
            socket_listener_.reset();
            socket_connection_handler_.reset();
            object_mapper_.reset();
            connection_handler_.reset();
            router_.reset();
            connection_provider_.reset();

            async_executor_->waitTasksFinished();
            async_executor_->stop();
            async_executor_->join();
            async_executor_.reset();

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