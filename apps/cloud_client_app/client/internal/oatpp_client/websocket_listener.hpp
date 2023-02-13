
#ifndef websocket_istener_hpp
#define websocket_istener_hpp

#include "oatpp-websocket/ConnectionHandler.hpp"
#include "oatpp-websocket/WebSocket.hpp"

#include <thread>

/**
 * WebSocket listener listens on incoming WebSocket events.
 */
class WebSocketListener : public oatpp::websocket::WebSocket::Listener
{
private:
  std::mutex &web_socket_write_mtx_;

private:
  /**
   * Buffer for messages. Needed for multi-frame messages.
   */
  oatpp::data::stream::BufferOutputStream m_messageBuffer;

public:
  WebSocketListener(std::mutex &web_socket_write_mtx) : web_socket_write_mtx_(web_socket_write_mtx)
  {
  }

  /**
   * Called on "ping" frame.
   */
  void onPing(const WebSocket &socket, const oatpp::String &message) override;

  /**
   * Called on "pong" frame
   */
  void onPong(const WebSocket &socket, const oatpp::String &message) override;

  /**
   * Called on "close" frame
   */
  void onClose(const WebSocket &socket, v_uint16 code, const oatpp::String &message) override;

  /**
   * Called on each message frame. After the last message will be called once-again with size == 0 to designate end of the message.
   */
  void readMessage(const WebSocket &socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size) override;
};

#endif // websocket_istener_hpp
