#include "network_module.hpp"

#include <thread>

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
                NetworkModuleImpl() = delete;
                NetworkModuleImpl(const int &available_processors_cores);
                ~NetworkModuleImpl() = default;

                bool start(const Config &config);
                void stop();

            private:
                const int kAvailableProcessorsCores_;

                bool is_running_;

                boost::asio::io_context io_context_;
                std::unique_ptr<Listener> listener_;

                std::vector<std::thread> workers_;
            };

            NetworkModule::NetworkModuleImpl::NetworkModuleImpl(const int &available_processors_cores)
                : kAvailableProcessorsCores_(available_processors_cores) {}

            bool NetworkModule::NetworkModuleImpl::start(const Config &config)
            {
                stop();

                LOG(DEBUG) << "Starting...";

                listener_ = std::make_unique<Listener>(kAvailableProcessorsCores_,
                                                       io_context_);
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

                workers_.reserve(kAvailableProcessorsCores_);
                for (int i = 0; i < kAvailableProcessorsCores_ - 1; ++i)
                {
                    workers_.push_back(std::thread([&]()
                                                   { io_context_.run(); }));
                }

                io_context_.run();
                // Not reaching this point

                is_running_ = true;
                return is_running_;
            }

            void NetworkModule::NetworkModuleImpl::stop()
            {
                LOG(DEBUG) << "Stopping...";

                if (!is_running_)
                {
                    LOG(DEBUG) << "Stopped";
                    return;
                }

                for (int i = 0; i < workers_.size(); i++)
                {
                    workers_[i].join();
                }
                workers_.clear();

                io_context_.stop();
                listener_.reset();

                is_running_ = false;

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