#include "../network_module.hpp"

#include <thread>

#include "easylogging++.h"
#include "json.hpp"

#include "http_session.hpp"

namespace
{
    bool is_error_important(const boost::system::error_code &error_code)
    {
        return !(error_code == boost::asio::error::operation_aborted);
    }
}

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
            ServerImpl();
            ~ServerImpl();

            bool start(const int &available_processors_cores,
                       const Server::Config &config);
            void stop();

        private:
            void listen_for_accept(const Server::Config &config);

        private:
            std::shared_ptr<boost::asio::io_context> io_context_;
            std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
            std::shared_ptr<boost::asio::ip::tcp::socket> socket_;

            std::vector<std::thread> workers_;
            std::mutex mutex_;
        };

        Server::ServerImpl::ServerImpl()
        {
        }

        Server::ServerImpl::~ServerImpl() {}

        bool Server::ServerImpl::start(const int &available_processors_cores,
                                       const Server::Config &config)
        {
            stop();

            boost::asio::ip::tcp::endpoint endpoint(
                {boost::asio::ip::make_address(config.host_)},
                config.port_);

            boost::system::error_code error_code;

            // Here compute number of threads

            // Creating

            io_context_.reset(new boost::asio::io_context(/* number of threads */));
            if (!io_context_)
            {
                LOG(ERROR) << "Can't create io_context";
                stop();
                return false;
            }
            acceptor_.reset(new boost::asio::ip::tcp::acceptor(*io_context_, endpoint));
            if (!acceptor_)
            {
                LOG(ERROR) << "Can't create acceptor";
                stop();
                return false;
            }
            socket_.reset(new boost::asio::ip::tcp::socket(*io_context_));
            if (!acceptor_)
            {
                LOG(ERROR) << "Can't create acceptor";
                stop();
                return false;
            }

            // Configuring

            acceptor_->set_option(boost::asio::socket_base::reuse_address(true));
            if (error_code)
            {
                LOG(ERROR) << "Can't set_option - (" << error_code.value() << ") " << error_code.message();
                stop();
                return false;
            }

            listen_for_accept(config);

            // Starting

            if (available_processors_cores < 1)
            {
                LOG(ERROR) << "Number of available cores in too small";
                stop();
                return false;
            }

            LOG(INFO) << "Starting " << available_processors_cores << " worker-threads...";

            if (available_processors_cores > 1)
            {
                workers_.reserve(available_processors_cores);

                for (int thread_i = 0; thread_i < (available_processors_cores - 1); ++thread_i)
                {
                    workers_.emplace_back(
                        [&]
                        {
                            {
                                const std::lock_guard<std::mutex> lock(mutex_);
                                LOG(INFO) << "Starting thread [" << std::this_thread::get_id() << "]";
                            }

                            io_context_->run();
                        });
                }
            }

            {
                const std::lock_guard<std::mutex> lock(mutex_);
                LOG(INFO) << "Starting thread [" << std::this_thread::get_id() << "]";
            }

            io_context_->run();

            return true;
        }

        void Server::ServerImpl::listen_for_accept(const Server::Config &config)
        {
            LOG(INFO) << "Listening for accepting...";

            acceptor_->async_accept(*socket_,
                                    [&](boost::beast::error_code error_code)
                                    {
                                        if (error_code)
                                        {
                                            if (is_error_important(error_code))
                                                LOG(ERROR) << "async_accept - (" << error_code.value() << ") " << error_code.message();
                                        }
                                        else
                                        {
                                            LOG(INFO) << "Creating new http connection...";
                                            std::make_shared<HttpSession>(std::move(*socket_), config.http_callbacks_)->start();
                                        }

                                        listen_for_accept(config);
                                    });
        }

        void Server::ServerImpl::stop()
        {
            LOG(DEBUG) << "Stopping...";

            if (!io_context_)
            {
                LOG(DEBUG) << "Stopped";
                return;
            }

            io_context_->stop();

            socket_.reset();
            acceptor_.reset();
            io_context_.reset();

            LOG(DEBUG) << "Stopped";
        }
    }
}
namespace network_module
{
    namespace server
    {
        Server::Server() : server_impl_(std::make_unique<ServerImpl>()) {}

        Server::~Server() {}

        bool Server::start(const int &available_processors_cores,
                           const Config &config)
        {
            LOG(INFO) << "Starting...";

            if (!server_impl_)
            {
                static const std::string kErrorText("Implementation is not created");
                LOG(ERROR) << kErrorText;
                throw std::runtime_error(kErrorText);
            }

            return server_impl_->start(available_processors_cores,
                                       config);
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
