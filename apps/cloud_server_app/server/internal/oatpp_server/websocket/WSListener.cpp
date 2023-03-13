
#include "WSListener.hpp"

oatpp::async::CoroutineStarter WSListener::onPing(const std::shared_ptr<AsyncWebSocket> &socket, const oatpp::String &message)
{
  return socket->sendPongAsync(message);
}

oatpp::async::CoroutineStarter WSListener::onPong(const std::shared_ptr<AsyncWebSocket> &socket, const oatpp::String &message)
{
  return nullptr;
}

oatpp::async::CoroutineStarter WSListener::onClose(const std::shared_ptr<AsyncWebSocket> &socket, v_uint16 code, const oatpp::String &message)
{
  return nullptr;
}

oatpp::async::CoroutineStarter WSListener::readMessage(const std::shared_ptr<AsyncWebSocket> &socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size)
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

void WSInstanceListener::onAfterCreate_NonBlocking(const std::shared_ptr<WSListener::AsyncWebSocket> &socket, const std::shared_ptr<const ParameterMap> &params)
{
  socket->setListener(std::make_shared<WSListener>());
}

void WSInstanceListener::onBeforeDestroy_NonBlocking(const std::shared_ptr<WSListener::AsyncWebSocket> &socket)
{
}