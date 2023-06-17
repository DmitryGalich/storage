#include "sessions_manager.hpp"

#include "easylogging++.h"

#include "websocket_session.hpp"
#include "http_session.hpp"

bool SessionsManager::add(std::shared_ptr<WebSocketSession> session)
{
    bool status = false;

    {
        std::lock_guard<std::mutex> lock(websocket_mutex_);
        status = websocket_sessions_.insert(session).second;
    }

    return status;
}

void SessionsManager::remove(WebSocketSession *session)
{
    for (auto iter = websocket_sessions_.begin(); iter != websocket_sessions_.end(); ++iter)
    {
        if ((*iter).get() == session)
        {
            std::lock_guard<std::mutex> lock(websocket_mutex_);
            websocket_sessions_.erase(*iter);
            break;
        }
    }
}

void SessionsManager::send(const std::string &message)
{
    auto const ss = std::make_shared<std::string const>(std::move(message));

    for (auto iter = websocket_sessions_.begin(); iter != websocket_sessions_.end(); ++iter)
    {
        (*iter)->send(ss);
    }
}

void SessionsManager::clear()
{
    websocket_sessions_.clear();
}
