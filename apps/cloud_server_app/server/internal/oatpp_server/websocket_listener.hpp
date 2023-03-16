#pragma once

#include "oatpp-websocket/AsyncConnectionHandler.hpp"
#include "oatpp-websocket/AsyncWebSocket.hpp"

class WebSocketListener : public oatpp::websocket::AsyncWebSocket::Listener
{
public:
  CoroutineStarter onPing(const std::shared_ptr<AsyncWebSocket> &socket, const oatpp::String &message) override;
  CoroutineStarter onPong(const std::shared_ptr<AsyncWebSocket> &socket, const oatpp::String &message) override;
  CoroutineStarter onClose(const std::shared_ptr<AsyncWebSocket> &socket, v_uint16 code, const oatpp::String &message) override;
  CoroutineStarter readMessage(const std::shared_ptr<AsyncWebSocket> &socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size) override;

private:
  oatpp::data::stream::BufferOutputStream m_messageBuffer;
};

class WebSocketInstanceListener : public oatpp::websocket::AsyncConnectionHandler::SocketInstanceListener
{
public:
  void onAfterCreate_NonBlocking(const std::shared_ptr<WebSocketListener::AsyncWebSocket> &socket, const std::shared_ptr<const ParameterMap> &params) override;
  void onBeforeDestroy_NonBlocking(const std::shared_ptr<WebSocketListener::AsyncWebSocket> &socket) override;
};