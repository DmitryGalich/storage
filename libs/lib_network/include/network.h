#pragma once

#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

namespace network {

class Server {
public:
  Server() = default;
  ~Server() { stop(); };

  void start(const std::string &ip, const uint32_t port) {
    static const int kMaxSocketsRequestsCount(4096);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    const int kServerAddressLength = sizeof(server_address);

    // --------------------------------------------

    if (inet_pton(AF_INET, ip.c_str(), &(server_address.sin_addr)) != 1)
      handle_error("inet_pton() - " + std::string(strerror(errno)),
                   HandlingError::WithoutClosing);

    server_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_ < 0)
      handle_error("socket() - " + std::string(strerror(errno)),
                   HandlingError::WithoutClosing);

    if (fcntl(server_, F_SETFL, O_NONBLOCK) < 0)
      handle_error("fcntl() - " + std::string(strerror(errno)));

    int opt = 1;
    if (setsockopt(server_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)) < 0)
      handle_error("setsockopt() - " + std::string(strerror(errno)));

    if (bind(server_, (struct sockaddr *)&server_address,
             sizeof(server_address)) < 0)
      handle_error("bind() - " + std::string(strerror(errno)));

    if (listen(server_, kMaxSocketsRequestsCount) < 0)
      handle_error("listen() - " + std::string(strerror(errno)));

    socket_descriptor client_socket =
        accept(server_, (struct sockaddr *)&server_address,
               (socklen_t *)&kServerAddressLength);
    if (client_socket < 0)
      handle_error("accept() - " + std::string(strerror(errno)));

    clients_.emplace_back(client_socket);

    // --------------------------------------------

    char buffer[1024] = {0};
    char hello[] = "Hello from server";

    if (read(client_socket, buffer, 1024) < 0)
      handle_error("read() - " + std::string(strerror(errno)));

    printf("%s\n", buffer);

    if (send(client_socket, hello, strlen(hello), 0) < 0)
      handle_error("send() - " + std::string(strerror(errno)));

    printf("Hello message sent\n");
  };

  void stop() {
    for (auto &client : clients_)
      if ((client >= 0) && (close(client) < 0))
        handle_error("close(" + std::to_string(client) + ") - " +
                         std::string(strerror(errno)),
                     HandlingError::WithoutClosing);

    if (server_ >= 0)
      if (close(server_) < 0)
        handle_error("close() - " + std::string(strerror(errno)),
                     HandlingError::WithoutClosing);
  };

private:
  typedef int socket_descriptor;

  enum class HandlingError : bool {
    WithClosing = true,
    WithoutClosing = false
  };

  void configure(){};

  void handle_error(
      const std::string error_message,
      const HandlingError is_need_closing = HandlingError::WithClosing) {

    if (is_need_closing == HandlingError::WithClosing)
      stop();

    throw std::runtime_error(error_message);
  };

  std::vector<socket_descriptor> clients_;
  socket_descriptor server_;
};

class Client {};

} // namespace network
