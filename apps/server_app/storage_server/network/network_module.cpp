#include "network_module.hpp"

#include "easylogging++.h"

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
                Config config_;
            };

            bool NetworkModule::NetworkModuleImpl::start(const Config &config) { return true; }

            void NetworkModule::NetworkModuleImpl::stop() {}

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