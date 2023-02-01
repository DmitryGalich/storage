#pragma once

#include <string>

namespace cloud
{
    struct ServerConfig
    {
        std::string network_lib_;
        std::string host_;
        int port_;
        bool is_ip_v6_family_;
        int executor_data_processing_threads_;
        int executor_io_threads_;
        int executor_timer_threads_;
    };

    namespace internal
    {
        class AbstractServer
        {
        public:
            virtual ~AbstractServer() {}

            virtual bool start() = 0;
            virtual void stop() = 0;
        };
    }
}
