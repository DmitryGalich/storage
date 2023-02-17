#pragma once

#include <string>

namespace server
{
    namespace config
    {
        struct Config
        {
            std::string host_;
            int port_;
            bool is_ip_v6_family_;
            int executor_data_processing_threads_;
            int executor_io_threads_;
            int executor_timer_threads_;
        };

        Config load_config(const std::string &path);
    }
}