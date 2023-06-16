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

    bool add(WebSocketSession *session);
    void remove(WebSocketSession *session);

    bool add(std::shared_ptr<HttpSession> session);
    void remove(HttpSession *session);

    void send(const std::string &message);

private:
    std::mutex websocket_mutex_;
    std::unordered_set<WebSocketSession *> websocket_sessions_;

    std::mutex http_mutex_;
    std::unordered_set<std::shared_ptr<HttpSession>> http_sessions_;
};