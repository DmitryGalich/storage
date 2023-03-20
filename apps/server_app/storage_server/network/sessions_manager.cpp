#include "sessions_manager.hpp"

#include "easylogging++.h"

#include "http_session.hpp"

bool SessionsManager::join(std::shared_ptr<HttpSession> session)
{
  if (!session)
  {
    LOG(ERROR) << "Http session is null";
    return false;
  }

  session->run();

  // Need to make thread-safe
  http_sessions_.insert(session);
}

void SessionsManager::leave(std::shared_ptr<HttpSession> session)
{
  // Need to make thread-safe

  http_sessions_.erase(session);
}

void SessionsManager::send(const std::string &message)
{
  // Need to make thread-safe

  // for (auto session : websocket_sessions_)
  //   session->send(message);
}
