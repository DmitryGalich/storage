#pragma once

#include "oatpp-websocket/ConnectionHandler.hpp"
#include "oatpp-websocket/WebSocket.hpp"

#include <thread>

class WebSocketListener : public oatpp::websocket::WebSocket::Listener
{
private:
  std::mutex &web_socket_write_mtx_;

private:
  oatpp::data::stream::BufferOutputStream m_messageBuffer;

public:
  WebSocketListener(std::mutex &web_socket_write_mtx) : web_socket_write_mtx_(web_socket_write_mtx)
  {
  }

  void onPing(const WebSocket &socket, const oatpp::String &message) override;

  void onPong(const WebSocket &socket, const oatpp::String &message) override;

  void onClose(const WebSocket &socket, v_uint16 code, const oatpp::String &message) override;

  void readMessage(const WebSocket &socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size) override;
};
