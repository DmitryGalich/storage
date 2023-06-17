#pragma once

#include <unordered_set>
#include <string>
#include <memory>
#include <mutex>

class WebSocketSession;
class HttpSession;

class SessionsManager
{
public:
    SessionsManager() = default;
    ~SessionsManager() = default;

    bool add(std::shared_ptr<WebSocketSession> session);
    void remove(WebSocketSession *session);

    void clear();

    void send(const std::string &message);

private:
    std::mutex websocket_mutex_;
    std::unordered_set<std::shared_ptr<WebSocketSession>> websocket_sessions_;
};