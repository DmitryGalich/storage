#pragma once

#include <memory>
#include <future>

namespace dummy
{
    namespace client
    {
        class Client
        {
        public:
            Client() = delete;
            Client(std::promise<void> signal_to_stop);
            ~Client();

            bool start(const std::string &config_path);
            void stop() noexcept;

            bool send(const std::string &message);

        private:
            class ClientImpl;
            std::unique_ptr<ClientImpl> client_impl_;
        };
    }
}
