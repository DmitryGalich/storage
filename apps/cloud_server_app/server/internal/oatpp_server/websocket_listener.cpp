
#include "websocket_listener.hpp"

oatpp::async::CoroutineStarter WebSocketListener::onPing(const std::shared_ptr<AsyncWebSocket> &socket, const oatpp::String &message)
{
  return socket->sendPongAsync(message);
}

oatpp::async::CoroutineStarter WebSocketListener::onPong(const std::shared_ptr<AsyncWebSocket> &socket, const oatpp::String &message)
{
  return nullptr;
}

oatpp::async::CoroutineStarter WebSocketListener::onClose(const std::shared_ptr<AsyncWebSocket> &socket, v_uint16 code, const oatpp::String &message)
{
  return nullptr;
}

oatpp::async::CoroutineStarter WebSocketListener::readMessage(const std::shared_ptr<AsyncWebSocket> &socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size)
{
  if (size == 0)
  { // message transfer finished
    auto wholeMessage = m_messageBuffer.toString();
    m_messageBuffer.setCurrentPosition(0);

    return socket->sendOneFrameTextAsync("Hello from oatpp!: " + wholeMessage);
  }
  else if (size > 0)
  { // message frame received
    m_messageBuffer.writeSimple(data, size);
  }

  return nullptr;
}

void WebSocketInstanceListener::onAfterCreate_NonBlocking(const std::shared_ptr<WebSocketListener::AsyncWebSocket> &socket, const std::shared_ptr<const ParameterMap> &params)
{
  socket->setListener(std::make_shared<WebSocketListener>());
}

void WebSocketInstanceListener::onBeforeDestroy_NonBlocking(const std::shared_ptr<WebSocketListener::AsyncWebSocket> &socket)
{
}