#include "oatpp_client.h"

#include "easylogging++.h"

#include "oatpp/core/base/Environment.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp-websocket/WebSocket.hpp"
#include "oatpp-websocket/Connector.hpp"

#include "../abstract_client.h"

#include <mutex>

namespace
{
    /**
     * WebSocket listener listens on incoming WebSocket events.
     */
    class WSListener : public oatpp::websocket::WebSocket::Listener
    {
    private:
        static constexpr const char *TAG = "Client_WSListener";

    private:
        std::mutex &m_writeMutex;
        /**
         * Buffer for messages. Needed for multi-frame messages.
         */
        oatpp::data::stream::BufferOutputStream m_messageBuffer;

    public:
        WSListener(std::mutex &writeMutex)
            : m_writeMutex(writeMutex)
        {
        }

        /**
         * Called on "ping" frame.
         */
        void onPing(const WebSocket &socket, const oatpp::String &message) override
        {
            OATPP_LOGD(TAG, "onPing");
            std::lock_guard<std::mutex> lock(m_writeMutex);
            socket.sendPong(message);
        }

        /**
         * Called on "pong" frame
         */
        void onPong(const WebSocket &socket, const oatpp::String &message) override
        {
            OATPP_LOGD(TAG, "onPong");
        }

        /**
         * Called on "close" frame
         */
        void onClose(const WebSocket &socket, v_uint16 code, const oatpp::String &message) override
        {
            OATPP_LOGD(TAG, "onClose code=%d", code);
        }

        /**
         * Called on each message frame. After the last message will be called once-again with size == 0 to designate end of the message.
         */
        void readMessage(const WebSocket &socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size) override
        {
            if (size == 0)
            { // message transfer finished

                auto wholeMessage = m_messageBuffer.toString();
                m_messageBuffer.setCurrentPosition(0);

                OATPP_LOGD(TAG, "on message received '%s'", wholeMessage->c_str());

                /* Send message in reply */
                // std::lock_guard<std::mutex> lock(m_writeMutex);
                // socket.sendOneFrameText( "Hello from oatpp!: " + wholeMessage);
            }
            else if (size > 0)
            { // message frame received
                m_messageBuffer.writeSimple(data, size);
            }
        }
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

            std::shared_ptr<oatpp::network::tcp::client::ConnectionProvider> connection_provider_;
            std::shared_ptr<oatpp::websocket::Connector> socket_connector_;
            std::shared_ptr<oatpp::websocket::WebSocket> socket_;
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

            auto connection = socket_connector_->connect("v3/channel_1?api_key=VCXCEuvhGcBDP7XhiJJUDvR1e1D3eiVjgZ9VRiaV&notify_self");
            LOG(INFO) << "Connected";

            socket_.reset();
            socket_ = oatpp::websocket::WebSocket::createShared(connection, true /* maskOutgoingMessages must be true for clients */);
            if (!socket_)
            {
                LOG(ERROR) << "WebSocket is not created";
                return false;
            }

            std::mutex socket_write_mutex;

            socket_->setListener(std::make_shared<WSListener>(socket_write_mutex));

            return true;
        }

        bool OatppClient::OatppClientImpl::run()
        {
            socket_->sendOneFrameText("hello");

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
            socket_.reset();
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