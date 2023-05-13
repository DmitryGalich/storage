#pragma once

#include <unordered_set>
#include <string>
#include <memory>

class WebSocketSession;

class SessionsManager
{
public:
    SessionsManager() = default;
    ~SessionsManager() = default;

    bool add(std::shared_ptr<WebSocketSession> session);
    void remove(std::shared_ptr<WebSocketSession> session);
    void send(const std::string &message);

private:
    // Need to make thread-safe
    std::unordered_set<std::shared_ptr<WebSocketSession>> sessions_;
};