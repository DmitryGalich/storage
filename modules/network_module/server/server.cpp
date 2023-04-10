#include "../network_module.hpp"

#include "easylogging++.h"

#include "json.hpp"

#include "http_session.hpp"

namespace network_module
{
    namespace server
    {
        Server::Config Server::Config::load_config(const std::string &config_path)
        {
            nlohmann::json json_object;
            json_object["host"] = "127.0.0.1";
            json_object["port"] = 8080;

            std::fstream file(config_path);
            if (!file.is_open())
            {
                LOG(INFO) << "Creating default config...";

                std::ofstream default_config_file(config_path);
                if (!default_config_file.is_open())
                {
                    const std::string kErrorText{"Can't save default config to \"" + config_path + "\""};
                    LOG(ERROR) << kErrorText;
                    throw std::runtime_error(kErrorText);
                }

                default_config_file << json_object.dump(4);
                default_config_file.close();

                LOG(INFO) << "Created";
            }
            else
            {
                json_object = nlohmann::json::parse(file);
                file.close();
            }

            LOG(INFO) << "Current config: \n"
                      << json_object.dump(4);

            network_module::server::Server::Config config;
            json_object.at("host").get_to(config.host_);
            json_object.at("port").get_to(config.port_);

            return config;
        }
    }
}
namespace network_module
{
    namespace server
    {
        class Server::ServerImpl
        {
        public:
            ServerImpl(const int &available_processors_cores);
            ~ServerImpl();

            bool start(const Server::Config &config);
            void stop();

        private:
            const int kAvailableProcessorsCores_;

            std::shared_ptr<boost::asio::io_context> io_context_;
            std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
            std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
        };

        Server::ServerImpl::ServerImpl(const int &available_processors_cores)
            : kAvailableProcessorsCores_(available_processors_cores)
        {
        }

        Server::ServerImpl::~ServerImpl() {}

        bool Server::ServerImpl::start(const Server::Config &config)
        {
            stop();

            return true;
        }

        void Server::ServerImpl::stop() {}
    }
}
namespace network_module
{
    namespace server
    {
        Server::Server(const int &available_processors_cores)
            : server_impl_(std::make_unique<ServerImpl>(available_processors_cores)) {}

        Server::~Server() {}

        bool Server::start(const Config &config)
        {
            LOG(INFO) << "Starting...";

            if (!server_impl_)
            {
                static const std::string kErrorText("Implementation is not created");
                LOG(ERROR) << kErrorText;
                throw std::runtime_error(kErrorText);
            }

            return server_impl_->start(config);
        }

        void Server::stop()
        {
            LOG(INFO) << "Stopping...";

            if (!server_impl_)
            {
                LOG(ERROR) << "Implementation is not created";
                return;
            }

            server_impl_->stop();

            LOG(INFO) << "Stopped";
        }
    }
}
