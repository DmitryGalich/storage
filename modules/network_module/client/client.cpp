#include "../network_module.hpp"

#include <memory>
#include <chrono>

#include "boost/asio/io_context.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/beast.hpp"
#include "boost/asio/strand.hpp"
#include "boost/enable_shared_from_this.hpp"
#include "boost/asio.hpp"
#include "boost/bind.hpp"
#include "boost/beast/websocket/stream_base.hpp"

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
            explicit ClientImpl();
            ~ClientImpl();

            bool start(const Config &config);
            void stop();

        private:
            void do_resolve(boost::beast::error_code error_code,
                            boost::asio::ip::tcp::resolver::results_type results, const Config &config);
            void do_connect(boost::beast::error_code error_code,
                            boost::asio::ip::tcp::resolver::results_type::endpoint_type endpoint, const Config &config);
            void do_handshake(boost::beast::error_code error_code);
            // void do_write(boost::beast::error_code ec, std::size_t bytes_transferred);
            // void do_read(boost::beast::error_code ec, std::size_t bytes_transferred);
            // void do_close(boost::beast::error_code ec);

        private:
            std::shared_ptr<boost::asio::io_context> io_context_;
            std::shared_ptr<boost::asio::ip::tcp::resolver> resolver_;
            std::shared_ptr<boost::beast::websocket::stream<boost::beast::tcp_stream>> websocket_stream_;

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

            resolver_.reset(new boost::asio::ip::tcp::resolver(*io_context_));
            if (!resolver_)
            {
                LOG(ERROR) << "Can't create resolver";
                stop();
                return false;
            }

            websocket_stream_.reset(new boost::beast::websocket::stream<boost::beast::tcp_stream>(*io_context_));
            if (!websocket_stream_)
            {
                LOG(ERROR) << "Can't create websocket_stream";
                stop();
                return false;
            }

            resolver_->async_resolve(
                config.host_.c_str(), std::to_string(config.port_),
                boost::bind(&Client::ClientImpl::do_resolve,
                            this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::results,
                            config));

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

            websocket_stream_.reset();

            resolver_.reset();
            io_context_.reset();

            LOG(INFO) << "Stopped";
        }

        void Client::ClientImpl::do_resolve(boost::beast::error_code error_code,
                                            boost::asio::ip::tcp::resolver::results_type results,
                                            const Config &config)
        {
            LOG(INFO) << "Resolving...";

            if (error_code)
            {
                LOG(ERROR) << "Error " << error_code;
            }

            // Set the timeout for the operation
            boost::beast::get_lowest_layer(*websocket_stream_).expires_after(std::chrono::seconds(30));
            // Make the connection on the IP address we get from a lookup
            boost::beast::get_lowest_layer(*websocket_stream_).async_connect(results, boost::bind(&Client::ClientImpl::do_connect, this, boost::asio::placeholders::error, boost::asio::placeholders::endpoint, config));
        }

        void Client::ClientImpl::do_connect(boost::beast::error_code error_code,
                                            boost::asio::ip::tcp::resolver::results_type::endpoint_type endpoint,
                                            const Config &config)
        {
            LOG(INFO) << "Connecting...";

            if (error_code)
            {
                LOG(ERROR) << "Error " << error_code;
            }

            // Turn off the timeout on the tcp_stream, because
            // the websocket stream has its own timeout system.
            boost::beast::get_lowest_layer(*websocket_stream_).expires_never();

            // Set suggested timeout settings for the websocket
            websocket_stream_->set_option(boost::beast::websocket::stream_base::timeout::suggested(
                boost::beast::role_type::client));

            // Set a decorator to change the User-Agent of the handshake
            websocket_stream_->set_option(boost::beast::websocket::stream_base::decorator(
                [](boost::beast::websocket::request_type &req)
                {
                    req.set(boost::beast::http::field::user_agent,
                            std::string(BOOST_BEAST_VERSION_STRING) +
                                " websocket-client-async");
                }));

            const std::string kHostAndPort = config.host_ + std::string(":") + std::to_string(config.port_);

            websocket_stream_->async_handshake(
                kHostAndPort.c_str(),
                "/",
                boost::bind(&Client::ClientImpl::do_handshake,
                            this,
                            boost::asio::placeholders::error));
        }

        void Client::ClientImpl::do_handshake(boost::beast::error_code error_code)
        {
            LOG(INFO) << "Handshaking...";

            if (error_code)
            {
                LOG(ERROR) << "Error " << error_code;
            }
        }

        // void Client::ClientImpl::do_write(boost::beast::error_code error_code, std::size_t bytes_transferred)
        // {
        // }

        // void Client::ClientImpl::do_read(boost::beast::error_code error_code, std::size_t bytes_transferred)
        // {
        // }

        // void Client::ClientImpl::do_close(boost::beast::error_code error_code)
        // {
        //     if (error_code)
        //     {
        //         LOG(ERROR) << "Error " << error_code;
        //     }

        //     LOG(INFO) << "Closing...";
        //     LOG(INFO) << boost::beast::make_printable(buffer_.data());
        // }

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
