#pragma once

#include <memory>

namespace storage
{
    class Client
    {
    public:
        Client();
        ~Client();

        bool start(const std::string &config_path);
        void stop() noexcept;

    private:
        class ClientImpl;
        std::unique_ptr<ClientImpl> client_impl_;
    };

} // namespace storage
