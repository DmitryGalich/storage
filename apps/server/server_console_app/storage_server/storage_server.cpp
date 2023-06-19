#include "storage_server.hpp"

#include "easylogging++.h"

#include "network_module.hpp"
#include "pages_manager/pages_manager.hpp"

namespace storage
{
    namespace server
    {
        class Server::ServerImpl
        {
        public:
            ServerImpl() = delete;
            ServerImpl(std::promise<void> signal_to_stop);
            ~ServerImpl();

            bool start(const int workers_number,
                       const std::string &config_path,
                       const std::string &html_folder_path);
            void stop() noexcept;

            bool send(const std::string &data);

        private:
            void configureCallbacks(network_module::server::Server::Config &config);

            void process_signal_to_stop();
            void process_new_websocket_connection();
            void process_receiving(const std::string &data);

        private:
            std::unique_ptr<network_module::server::Server> network_module_;
            std::unique_ptr<PagesManager> pages_manager_;

            std::atomic_bool is_stop_signal_called_{false};
            std::promise<void> signal_to_stop_;
        };

        Server::ServerImpl::ServerImpl(std::promise<void> signal_to_stop)
            : signal_to_stop_(std::move(signal_to_stop)) {}

        Server::ServerImpl::~ServerImpl()
        {
            stop();
        }

        bool Server::ServerImpl::start(const int workers_number,
                                       const std::string &config_path,
                                       const std::string &html_folder_path)
        {
            LOG(INFO) << "Starting...";

            network_module_.reset(new network_module::server::Server());
            if (!network_module_)
            {
                LOG(ERROR) << "Can't create network module";
                return false;
            }

            pages_manager_.reset(new PagesManager(html_folder_path));
            if (!pages_manager_)
            {
                LOG(ERROR) << "Can't create pages modules";
                return false;
            }

            auto config =
                network_module::server::Server::Config::load_config(config_path);

            configureCallbacks(config);

            if (!network_module_->start(workers_number,
                                        config))
                return false;

            return true;
        }

        void Server::ServerImpl::stop() noexcept
        {
            LOG(INFO) << "Stopping...";

            pages_manager_.reset();

            if (network_module_)
            {
                network_module_->stop();
            }
            network_module_.reset();

            LOG(INFO) << "Stopped";
        }

        void Server::ServerImpl::configureCallbacks(network_module::server::Server::Config &config)
        {
            // General
            {
                is_stop_signal_called_ = false;
                config.callbacks_.signal_to_stop_ = std::bind(&Server::ServerImpl::process_signal_to_stop, this);
            }

            // Html callbacks
            {
                config.callbacks_.http_callbacks_["/"] = [&]()
                { return pages_manager_->getHomePage(); };

                config.callbacks_.http_callbacks_["/kek"] = [&]()
                { return pages_manager_->getKekPage(); };

                config.callbacks_.http_callbacks_[network_module::Urls::kPageNotFound_] = [&]()
                { return pages_manager_->getPageNotFoundPage(); };
            }

            // Websockets
            {
                config.callbacks_.web_sockets_callbacks_.process_new_connection_ = std::bind(&Server::ServerImpl::process_new_websocket_connection, this);
                config.callbacks_.web_sockets_callbacks_.process_receiving_ = std::bind(&Server::ServerImpl::process_receiving, this, std::placeholders::_1);
            }
        }

        void Server::ServerImpl::process_signal_to_stop()
        {
            if (is_stop_signal_called_)
                return;

            is_stop_signal_called_ = true;
            signal_to_stop_.set_value();
        }

        void Server::ServerImpl::process_new_websocket_connection()
        {
            LOG(INFO) << "New websocket connection";
        }

        void Server::ServerImpl::process_receiving(const std::string &data)
        {
            LOG(INFO) << "Received: " << data;
        }

        bool Server::ServerImpl::send(const std::string &data)
        {
            if (!network_module_)
            {
                LOG(ERROR) << "Network module is not initialized";
                return false;
            }

            return network_module_->send(data);
        }

    }
}

namespace storage
{
    namespace server
    {
        Server::Server(std::promise<void> signal_to_stop) : server_impl_(std::make_unique<storage::server::Server::ServerImpl>(std::move(signal_to_stop))) {}

        Server::~Server() {}

        bool Server::start(const int workers_number,
                           const std::string &config_path,
                           const std::string &html_folder_path)
        {
            if (!server_impl_)
            {
                static const std::string kErrorText("Implementation is not created");
                LOG(ERROR) << kErrorText;
                throw std::runtime_error(kErrorText);
            }

            return server_impl_->start(workers_number,
                                       config_path,
                                       html_folder_path);
        }

        void Server::stop() noexcept
        {
            if (!server_impl_)
            {
                LOG(ERROR) << "Implementation is not created";
                return;
            }

            server_impl_->stop();
        }

        bool Server::send(const std::string &data)
        {
            if (!server_impl_)
            {
                static const std::string kErrorText("Implementation is not created");
                LOG(ERROR) << kErrorText;
                throw std::runtime_error(kErrorText);
            }

            return server_impl_->send(data);
        }
    }
}
