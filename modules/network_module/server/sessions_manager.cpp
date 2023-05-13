#include "sessions_manager.hpp"

#include "easylogging++.h"

#include "websocket_session.hpp"

bool SessionsManager::add(WebSocketSession *session)
{
    if (!session)
    {
        LOG(ERROR) << "WebSocket session is null";
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    sessions_.insert(session);
}

void SessionsManager::remove(WebSocketSession *session)
{
    if (!session)
    {
        LOG(ERROR) << "WebSocket session is null";
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.insert(session);
}

void SessionsManager::send(const std::string &message)
{
    auto const ss = std::make_shared<std::string const>(std::move(message));

    for (auto session : sessions_)
        session->send(ss);
}
