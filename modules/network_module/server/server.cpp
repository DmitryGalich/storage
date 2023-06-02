#include "../network_module.hpp"

#include <thread>
#include <functional>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "easylogging++.h"
#define ELPP_THREAD_SAFE

#include "json.hpp"

#include "http_session.hpp"
#include "sessions_manager.hpp"

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

            bool start(const int &workers_number,
                       const Server::Config &config);
            void stop();

            bool send(const std::string &data);

        private:
            void accept(const Server::Config &config);
            void on_accept(const boost::system::error_code &error, const Server::Config &config);

        private:
            std::unique_ptr<const Config> config_;

            std::mutex connecting_mutex_;
            std::condition_variable connecting_watcher_;

            std::shared_ptr<boost::asio::io_context> io_context_;
            std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
            std::shared_ptr<boost::asio::ip::tcp::socket> socket_;

            SessionsManager session_manager_;

            std::vector<std::thread> workers_;
            std::mutex mutex_;
        };

        Server::ServerImpl::ServerImpl()
        {
        }

        Server::ServerImpl::~ServerImpl() {}

        bool Server::ServerImpl::start(const int &workers_number,
                                       const Server::Config &config)
        {
            LOG(DEBUG) << "Starting...";

            if (is_running())
            {
                LOG(WARNING) << "Server is already running";
                return false;
            }

            config_ = std::make_unique<const Config>(config);

            stop();

            boost::asio::ip::tcp::endpoint endpoint(
                {boost::asio::ip::make_address(config.host_)},
                config.port_);

            boost::system::error_code error_code;

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
            if (!socket_)
            {
                LOG(ERROR) << "Can't create socket";
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

            accept(config);

            // Starting

            if (workers_number < 1)
            {
                LOG(ERROR) << "Number of available cores in too small";
                stop();
                return false;
            }

            LOG(DEBUG) << "Starting " << workers_number << " worker-threads...";

            workers_.reserve(workers_number);

            for (int thread_i = 0; thread_i < workers_number; ++thread_i)
            {
                workers_.emplace_back(
                    [&]
                    {
                        {
                            const std::lock_guard<std::mutex> lock(mutex_);
                            LOG(DEBUG) << "Starting worker [" << std::this_thread::get_id() << "]";
                        }

                        io_context_->run();
                    });
            }

            LOG(DEBUG) << "Started";
            return true;
        }

        void Server::ServerImpl::stop()
        {
            LOG(INFO) << "Stopping...";

            if (!io_context_)
            {
                LOG(INFO) << "Stopped";
                return;
            }
            io_context_->stop();

            int worker_i = 0;
            for (auto &worker : workers_)
            {
                LOG(INFO) << "Worker(" << worker_i << ") stopping...";
                worker.join();
                LOG(INFO) << "Worker(" << worker_i++ << ") stopped";
            }
            workers_.clear();

            socket_.reset();
            acceptor_.reset();
            io_context_.reset();

            LOG(INFO) << "Stopped";
        }

        void Server::ServerImpl::accept(const Server::Config &config)
        {
            LOG(DEBUG);

            acceptor_->async_accept(*socket_, boost::bind(&Server::ServerImpl::on_accept, this,
                                                          boost::asio::placeholders::error, config));
        }

        void Server::ServerImpl::on_accept(const boost::system::error_code &error_code, const Server::Config &config)
        {
            LOG(DEBUG);

            if (error_code)
            {
                if (is_error_important(error_code))
                    LOG(ERROR) << "async_accept - (" << error_code.value() << ") " << error_code.message();
            }
            else
            {
                LOG(DEBUG) << "Creating new http connection...";
                std::make_shared<HttpSession>(std::move(*socket_), session_manager_, config.callbacks_.http_callbacks_)->start();
            }

            accept(config);
        }

        bool Server::ServerImpl::send(const std::string &data)
        {
            return true;
        }
    }
}

namespace network_module
{
    namespace server
    {
        Server::Server() : server_impl_(std::make_unique<ServerImpl>()) {}

        Server::~Server() {}

        bool Server::start(const int &workers_number,
                           const Config &config)
        {
            LOG(INFO) << "Starting...";

            if (!server_impl_)
            {
                static const std::string kErrorText("Implementation is not created");
                LOG(ERROR) << kErrorText;
                throw std::runtime_error(kErrorText);
            }

            return server_impl_->start(workers_number,
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

        bool Server::send(const std::string &data)
        {
            if (!server_impl_)
            {
                LOG(ERROR) << "Implementation is not created";
                return false;
            }

            return server_impl_->send(data);
        }
    }
}
