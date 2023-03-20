#pragma once

#include <unordered_set>
#include <string>
#include <memory>

class HttpSession;

class SessionsManager
{
public:
    SessionsManager() = default;
    ~SessionsManager() = default;

    bool add(std::shared_ptr<HttpSession> session);
    void remove(std::shared_ptr<HttpSession> session);
    void send(const std::string &message);

private:
    // Need to make thread-safe

    std::unordered_set<std::shared_ptr<HttpSession>> http_sessions_;
};