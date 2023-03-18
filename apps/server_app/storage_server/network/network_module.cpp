#include "network_module.hpp"

#include "easylogging++.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>

#include "listener.hpp"

namespace storage
{
    namespace server
    {
        namespace network
        {
            class NetworkModule::NetworkModuleImpl
            {
            public:
                NetworkModuleImpl() = default;
                ~NetworkModuleImpl() = default;

                bool start(const Config &config);
                void stop();

            private:
                bool is_running_;

                boost::asio::io_context io_context_;
                std::unique_ptr<Listener> listener_;
            };

            bool NetworkModule::NetworkModuleImpl::start(const Config &config)
            {
                stop();

                listener_ = std::make_unique<Listener>(io_context_);
                if (!listener_)
                {
                    LOG(ERROR) << "Can't create Listener";
                    return false;
                }

                boost::asio::ip::tcp::endpoint kEndpoint(
                    {boost::asio::ip::make_address(config.host_)},
                    config.port_);

                if (!listener_->run(kEndpoint))
                {
                    LOG(ERROR) << "Can't run Listener";
                    return false;
                }

                io_context_.run();
                is_running_ = true;

                return is_running_;
            }

            void NetworkModule::NetworkModuleImpl::stop()
            {
                if (!is_running_)
                    return;

                io_context_.stop();
                listener_.reset();

                is_running_ = false;
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
            NetworkModule::NetworkModule() : network_module_impl_(std::make_unique<storage::server::network::NetworkModule::NetworkModuleImpl>()) {}

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