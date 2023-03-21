#pragma once

#include <memory>

namespace storage
{
    class Server
    {
    public:
        Server() = delete;
        Server(const int available_processors_cores);
        ~Server();

        bool start(const std::string &config_path);
        void stop() noexcept;

    private:
        class ServerImpl;
        std::unique_ptr<ServerImpl> server_impl_;
    };

} // namespace storage
