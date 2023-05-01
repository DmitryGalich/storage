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
            void process_resolve(boost::beast::error_code error_code,
                                 boost::asio::ip::tcp::resolver::results_type results);
            void process_connect(boost::beast::error_code error_code,
                                 boost::asio::ip::tcp::resolver::results_type::endpoint_type endpoint_type);
            void process_handshake(boost::beast::error_code error_code);
            void process_write(boost::beast::error_code ec, std::size_t bytes_transferred);
            void process_read(boost::beast::error_code ec, std::size_t bytes_transferred);
            void process_close(boost::beast::error_code ec);

        private:
            std::shared_ptr<boost::asio::io_context> io_context_;
            std::shared_ptr<boost::asio::ip::tcp::resolver> resolver_;
            std::shared_ptr<boost::beast::websocket::stream<boost::beast::tcp_stream>> websocket_stream_;
            boost::beast::flat_buffer buffer_;
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

            websocket_stream_.reset(new boost::beast::websocket::stream<boost::beast::tcp_stream>(*io_context_.get()));
            if (!websocket_stream_)
            {
                LOG(ERROR) << "Can't create tcp_stream";
                stop();
                return false;
            }

            // resolver_->async_resolve(
            //     config.host_,
            //     config.port_, [&](boost::beast::error_code error_code)
            //     { process_resolve(error_code); });

            // resolver_->async_resolve(
            //     config.host_,
            //     config.port_,
            //     boost::beast::bind_front_handler(
            //         &Client::ClientImpl::process_resolve,
            //         shared_from_this()));

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

            websocket_stream_.reset();
            resolver_.reset();
            io_context_.reset();

            LOG(INFO) << "Stopped";
        }

        void Client::ClientImpl::process_resolve(boost::beast::error_code error_code,
                                                 boost::asio::ip::tcp::resolver::results_type results)
        {
            if (error_code)
            {
                LOG(ERROR) << "Error " << error_code;
            }

            // Set the timeout for the operation
            boost::beast::get_lowest_layer(*websocket_stream_.get()).expires_after(std::chrono::seconds(30));

            // Make the connection on the IP address we get from a lookup
            // boost::beast::get_lowest_layer(websocket_stream_).async_connect(results, boost::beast::bind_front_handler(&Client::ClientImpl::process_connect, shared_from_this()));
        }

        void Client::ClientImpl::process_connect(boost::beast::error_code error_code,
                                                 boost::asio::ip::tcp::resolver::results_type::endpoint_type endpoint_type)
        {
        }

        void Client::ClientImpl::process_handshake(boost::beast::error_code error_code)
        {
        }

        void Client::ClientImpl::process_write(boost::beast::error_code error_code, std::size_t bytes_transferred)
        {
        }

        void Client::ClientImpl::process_read(boost::beast::error_code error_code, std::size_t bytes_transferred)
        {
        }

        void Client::ClientImpl::process_close(boost::beast::error_code error_code)
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
