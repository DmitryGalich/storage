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
            ServerImpl() = default;
            ~ServerImpl() = default;

            bool start(const int workers_number,
                       const std::string &config_path,
                       const std::string &html_folder_path);
            void stop() noexcept;

        private:
            void configureHtmlCallbacks(network_module::server::Server::Config &config);
            void configureWebSocketCallbacks(network_module::server::Server::Config &config);

        private:
            std::unique_ptr<network_module::server::Server> network_module_;
            std::unique_ptr<PagesManager> pages_manager_;
        };

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

            configureHtmlCallbacks(config);
            configureWebSocketCallbacks(config);

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

        void Server::ServerImpl::configureHtmlCallbacks(network_module::server::Server::Config &config)
        {
            config.http_callbacks_["/"] = [&]()
            { return pages_manager_->getHomePage(); };

            config.http_callbacks_["/kek"] = [&]()
            { return pages_manager_->getKekPage(); };

            config.http_callbacks_[network_module::Urls::kPageNotFound_] = [&]()
            { return pages_manager_->getPageNotFoundPage(); };
        }

        void Server::ServerImpl::configureWebSocketCallbacks(network_module::server::Server::Config &config)
        {
            config.receiving_callback_ = [&](const std::string &data)
            {
                LOG(INFO) << "Received data: " << data;
            };
        }
    }
}

namespace storage
{
    namespace server
    {
        Server::Server() : server_impl_(std::make_unique<storage::server::Server::ServerImpl>()) {}

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
    }
}
