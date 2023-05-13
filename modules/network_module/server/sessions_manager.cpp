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

    sessions_.insert(std::shared_ptr<WebSocketSession>(session));
}

void SessionsManager::remove(WebSocketSession *session)
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
