#include "network_module.hpp"

#include <thread>

#include "easylogging++.h"

#include "http_session.hpp"

namespace
{
    bool is_error_important(const boost::system::error_code &error_code)
    {
        return !(error_code == boost::asio::error::operation_aborted);
    }
}

namespace storage
{
    namespace server
    {
        namespace network
        {

            class NetworkModule::NetworkModuleImpl
            {
            public:
                NetworkModuleImpl() = delete;
                NetworkModuleImpl(const int &available_processors_cores);
                ~NetworkModuleImpl() = default;

                bool start(const Config &config);
                void stop();

            private:
                void listen_for_accept();

            private:
                const int kAvailableProcessorsCores_;

                std::shared_ptr<boost::asio::io_context> io_context_;
                std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
                std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
            };

            NetworkModule::NetworkModuleImpl::NetworkModuleImpl(const int &available_processors_cores)
                : kAvailableProcessorsCores_(available_processors_cores) {}

            bool NetworkModule::NetworkModuleImpl::start(const Config &config)
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

                listen_for_accept();

                io_context_->run();

                return true;
            }

            void NetworkModule::NetworkModuleImpl::listen_for_accept()
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
                                                std::make_shared<HttpSession>(std::move(*socket_))->start();
                                            }

                                            listen_for_accept();
                                        });
            }

            void NetworkModule::NetworkModuleImpl::stop()
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
}

namespace storage
{
    namespace server
    {
        namespace network
        {
            NetworkModule::NetworkModule(const int &available_processors_cores) : network_module_impl_(std::make_unique<storage::server::network::NetworkModule::NetworkModuleImpl>(available_processors_cores)) {}

            NetworkModule::~NetworkModule() {}

            bool NetworkModule::start(const Config &config)
            {
                LOG(INFO) << "Starting...";

                if (!network_module_impl_)
                {
                    static const std::string kErrorText("Implementation is not created");
                    LOG(ERROR) << kErrorText;
                    throw std::runtime_error(kErrorText);
                }

                return network_module_impl_->start(config);
            }

            void NetworkModule::stop()
            {
                LOG(INFO) << "Stopping...";

                if (!network_module_impl_)
                {
                    LOG(ERROR) << "Implementation is not created";
                    return;
                }

                network_module_impl_->stop();

                LOG(INFO) << "Stopped";
            }
        }
    }
}