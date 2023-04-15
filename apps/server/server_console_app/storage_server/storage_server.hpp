#pragma once

#include <memory>

namespace storage
{
    namespace server
    {
        class Server
        {
        public:
            Server();
            ~Server();

            bool start(const int available_processors_cores,
                       const std::string &config_path,
                       const std::string &html_folder_path);
            void stop() noexcept;

        private:
            class ServerImpl;
            std::unique_ptr<ServerImpl> server_impl_;
        };
    }
}
