#include "sessions_manager.hpp"

#include "easylogging++.h"

#include "websocket_session.hpp"

void SessionsManager::add(WebSocketSession &session)
{
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.insert(&session);
}

void SessionsManager::remove(WebSocketSession &session)
{
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.erase(&session);
}

void SessionsManager::send(const std::string &message)
{
    auto const ss = std::make_shared<std::string const>(std::move(message));

    for (WebSocketSession *session : sessions_)
        session->send(ss);
}
