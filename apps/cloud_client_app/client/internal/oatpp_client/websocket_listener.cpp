
#include "websocket_listener.hpp"

void WebSocketListener::onPing(const WebSocket &socket, const oatpp::String &message)
{
  OATPP_LOGD("KEK", "onPing");
  std::lock_guard<std::mutex> lock(web_socket_write_mtx_);

  socket.sendPong(message);
}

void WebSocketListener::onPong(const WebSocket &socket, const oatpp::String &message)
{
  OATPP_LOGD("KEK", "onPong");
}

void WebSocketListener::onClose(const WebSocket &socket, v_uint16 code, const oatpp::String &message)
{
  OATPP_LOGD("KEK", "onClose code=%d", code);
}

void WebSocketListener::readMessage(const WebSocket &socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size)
{

  if (size == 0)
  { // message transfer finished

    auto wholeMessage = m_messageBuffer.toString();
    m_messageBuffer.setCurrentPosition(0);

    OATPP_LOGD("KEK", "on message received '%s'", wholeMessage->c_str());
  }
  else if (size > 0)
  { // message frame received
    m_messageBuffer.writeSimple(data, size);
  }
}
