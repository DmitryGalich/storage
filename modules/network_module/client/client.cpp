#include "../network_module.hpp"

#include <memory>
#include <chrono>

#include "boost/asio/io_context.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/beast.hpp"
#include "boost/asio/strand.hpp"

#include "boost/asio.hpp"
#include "boost/bind.hpp"

#include "easylogging++.h"
#include "json.hpp"

namespace
{
    bool is_error_important(const boost::system::error_code &error_code)
    {
        return !(error_code == boost::asio::error::operation_aborted);
    }
}

namespace network_module
{
    namespace client
    {
        Client::Config Client::Config::load_config(const std::string &config_path)
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

            network_module::client::Client::Config config;
            json_object.at("host").get_to(config.host_);
            json_object.at("port").get_to(config.port_);

            return config;
        }
    }
}
namespace network_module
{
    namespace client
    {
        class Client::ClientImpl : public std::enable_shared_from_this<Client::ClientImpl>
        {
        public:
            ClientImpl();
            ~ClientImpl();

            bool start(const Config &config);
            void stop();

        private:
            void do_resolve(boost::beast::error_code error_code,
                            boost::asio::ip::tcp::resolver::iterator iterator);
            void do_connect(boost::beast::error_code error_code,
                            boost::asio::ip::tcp::resolver::iterator iterator);
            void do_handshake(boost::beast::error_code error_code);
            void do_write(boost::beast::error_code ec, std::size_t bytes_transferred);
            void do_read(boost::beast::error_code ec, std::size_t bytes_transferred);
            void do_close(boost::beast::error_code ec);

        private:
            std::shared_ptr<boost::asio::io_context> io_context_;
            std::shared_ptr<boost::asio::ip::tcp::resolver> resolver_;
            std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
            boost::beast::flat_buffer buffer_;

            std::vector<std::thread> workers_;
            std::mutex mutex_;
        };

        Client::ClientImpl::ClientImpl() {}

        Client::ClientImpl::~ClientImpl() {}

        bool Client::ClientImpl::start(const Config &config)
        {
            stop();

            LOG(INFO) << "Starting...";

            io_context_.reset(new boost::asio::io_context(/* number of threads */));
            if (!io_context_)
            {
                LOG(ERROR) << "Can't create io_context";
                stop();
                return false;
            }

            resolver_.reset(new boost::asio::ip::tcp::resolver(*io_context_.get()));
            if (!resolver_)
            {
                LOG(ERROR) << "Can't create resolver";
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

            resolver_->async_resolve(config.host_.c_str(), std::to_string(config.port_),
                                     boost::bind(&Client::ClientImpl::do_resolve,
                                                 this,
                                                 boost::asio::placeholders::error,
                                                 boost::asio::placeholders::results));

            const int kWorkersNumber = 1;
            if (kWorkersNumber < 1)
            {
                LOG(ERROR) << "Number of available cores in too small";
                stop();
                return false;
            }

            LOG(INFO) << "Starting " << kWorkersNumber << " worker-threads...";

            workers_.reserve(kWorkersNumber);

            for (int thread_i = 0; thread_i < kWorkersNumber; ++thread_i)
            {
                workers_.emplace_back(
                    [&]
                    {
                        {
                            const std::lock_guard<std::mutex> lock(mutex_);
                            LOG(INFO) << "Starting worker [" << std::this_thread::get_id() << "]";
                        }

                        io_context_->run();
                    });
            }

            return true;
        }

        void Client::ClientImpl::stop()
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
            resolver_.reset();
            io_context_.reset();

            LOG(INFO) << "Stopped";
        }

        void Client::ClientImpl::do_resolve(boost::beast::error_code error_code,
                                            boost::asio::ip::tcp::resolver::iterator iterator)
        {
            LOG(INFO) << "Process resolving...";

            if (error_code)
            {
                LOG(ERROR) << "Error " << error_code;
            }

            boost::asio::ip::tcp::endpoint end_point = *iterator;
            socket_->async_connect(end_point, boost::bind(&Client::ClientImpl::do_connect, this,
                                                          boost::asio::placeholders::error,
                                                          ++iterator));
        }

        void Client::ClientImpl::do_connect(boost::beast::error_code error_code,
                                            boost::asio::ip::tcp::resolver::iterator iterator)
        {
            LOG(INFO) << "Process connecting...";

            if (error_code)
            {
                LOG(ERROR) << "Error " << error_code;
            }
        }

        void Client::ClientImpl::do_handshake(boost::beast::error_code error_code)
        {
        }

        void Client::ClientImpl::do_write(boost::beast::error_code error_code, std::size_t bytes_transferred)
        {
        }

        void Client::ClientImpl::do_read(boost::beast::error_code error_code, std::size_t bytes_transferred)
        {
        }

        void Client::ClientImpl::do_close(boost::beast::error_code error_code)
        {
            if (error_code)
            {
                LOG(ERROR) << "Error " << error_code;
            }

            LOG(INFO) << "Closing...";
            LOG(INFO) << boost::beast::make_printable(buffer_.data());
        }
    }
}

namespace network_module
{
    namespace client
    {
        Client::Client() : client_impl_(std::make_unique<ClientImpl>()) {}

        Client::~Client() {}

        bool Client::start(const Config &config)
        {
            LOG(INFO) << "Starting...";

            if (!client_impl_)
            {
                static const std::string kErrorText("Implementation is not created");
                LOG(ERROR) << kErrorText;
                throw std::runtime_error(kErrorText);
            }

            return client_impl_->start(config);
        }

        void Client::stop()
        {
            LOG(INFO) << "Stopping...";

            if (!client_impl_)
            {
                LOG(ERROR) << "Implementation is not created";
                return;
            }

            client_impl_->stop();

            LOG(INFO) << "Stopped";
        }
    }
}
