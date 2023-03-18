#pragma once

#include <unordered_set>

class websocket_session;

class SessionsManager
{
public:
    SessionsManager() = default;
    ~SessionsManager() = default;

    void join(websocket_session &session);
    void leave(websocket_session &session);
    void send(const std::string &message);

private:
    // Need to make thread-safe
    std::unordered_set<websocket_session *> websocket_sessions_;
};