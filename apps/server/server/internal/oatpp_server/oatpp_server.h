#pragma once

#include "../abstract_server.h"

#include <memory>

namespace cloud
{
    namespace internal
    {
        class OatppServer : public AbstractServer
        {
        public:
            OatppServer(const ServerConfig &config);
            ~OatppServer() override;

            bool start() override;
            void stop() override;

        private:
            class OatppServerImpl;
            std::unique_ptr<OatppServerImpl> server_impl_;
        };
    }
}
