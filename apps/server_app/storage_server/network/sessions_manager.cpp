#include "sessions_manager.hpp"

#include "websocket_session.hpp"

void SessionsManager::join(websocket_session &session)
{
  // Need to make thread-safe

  // sessions_.insert(&session);
}

void SessionsManager::leave(websocket_session &session)
{
  // Need to make thread-safe

  // sessions_.erase(&session);
}

void SessionsManager::send(const std::string &message)
{
  // Need to make thread-safe

  // for (auto session : websocket_sessions_)
  //   session->send(message);
}
