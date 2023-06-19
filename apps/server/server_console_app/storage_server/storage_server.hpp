#pragma once

#include <memory>
#include <future>

namespace storage
{
    namespace server
    {
        class Server
        {
        public:
            Server() = delete;
            Server(std::promise<void> signal_to_stop);
            ~Server();

            bool start(const int workers_number,
                       const std::string &config_path,
                       const std::string &html_folder_path);
            void stop() noexcept;

            bool send(const std::string &data);

        private:
            class ServerImpl;
            std::unique_ptr<ServerImpl> server_impl_;
        };
    }
}
