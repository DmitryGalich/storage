#pragma once

#include <memory>

namespace cloud
{
    class Server
    {
    public:
        Server();
        ~Server();

        bool start(const std::string &config_path);
        void stop() noexcept;

    private:
        class ServerImpl;
        std::unique_ptr<ServerImpl> server_impl_;
    };
}
