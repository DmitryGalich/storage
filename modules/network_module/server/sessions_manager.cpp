#include "sessions_manager.hpp"

#include "easylogging++.h"

bool SessionsManager::add(std::shared_ptr<WebSocketSession> session)
{
    if (!session)
    {
        LOG(ERROR) << "WebSocket session is null";
        return false;
    }

    sessions_.insert(session);
}

void SessionsManager::remove(std::shared_ptr<WebSocketSession> session)
{
    // Need to make thread-safe

    if (!session)
    {
        LOG(ERROR) << "WebSocket session is null";
        return;
    }
}

void SessionsManager::send(const std::string &message)
{
    // Need to make thread-safe

    // for (auto session : websocket_sessions_)
    //   session->send(message);
}
