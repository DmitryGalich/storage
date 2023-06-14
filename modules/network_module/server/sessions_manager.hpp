#pragma once

#include <unordered_set>
#include <string>
#include <memory>
#include <mutex>

class WebSocketSession;

class SessionsManager
{
public:
    SessionsManager() = default;
    ~SessionsManager() = default;

    void add(WebSocketSession &session);
    void remove(WebSocketSession &session);

    void send(const std::string &message);

private:
    std::mutex mutex_;
    std::unordered_set<WebSocketSession *> sessions_;
};