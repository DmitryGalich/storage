#pragma once

#include <memory>

// class WebSocketSession : public std::enable_shared_from_this<WebSocketSession>
// {
// public:
//     WebSocketSession() = delete;
//     WebSocketSession(SessionsManager &sessions_manager);
//     ~WebSocketSession() = default;

//     void run();

// private:
//     void process_fail(const error_code &error_code,
//                       char const *reason);
//     void process_read(const error_code &eerror_codec,
//                       std::size_t);
//     void process_write(const error_code &error_code,
//                        std::size_t,
//                        bool is_need_close);

// private:
// };