#include "../network_module.hpp"

#include <memory>
#include <chrono>
#include <vector>
#include <thread>
#include <condition_variable>

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
    static const bool kReconnecting{true};
    static const bool kNotReconnecting{false};

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
            json_object["reconnect_timeout_sec"] = 5;

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
            json_object.at("reconnect_timeout_sec").get_to(config.reconnect_timeout_sec_);

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
            bool is_running() const;

            bool send(const std::string &data);

        private:
            void run_general_thread();

        private:
            bool is_started() const;

            void resolve(const Config &config,
                         const bool is_reconnecting = kNotReconnecting);
            void on_resolve(boost::beast::error_code error_code,
                            boost::asio::ip::tcp::resolver::results_type results,
                            const Config &config);
            void on_connect(boost::beast::error_code error_code,
                            boost::asio::ip::tcp::resolver::results_type::endpoint_type endpoint,
                            const Config &config);
            void on_handshake(boost::beast::error_code error_code,
                              const Config &config);
            void on_send(boost::beast::error_code error_code,
                         std::size_t bytes_transferred);

            void listen(const Config &config);
            void on_receive(boost::beast::error_code error_code,
                            std::size_t bytes_transferred,
                            const Config &config);

            void close(const Config &config,
                       bool is_reconnecting);

        private:
            std::shared_ptr<boost::asio::io_context> io_context_;
            std::shared_ptr<boost::asio::ip::tcp::resolver> resolver_;
            std::shared_ptr<boost::beast::websocket::stream<boost::beast::tcp_stream>> websocket_stream_;

            Config::Callbacks callbacks_;

            boost::beast::flat_buffer buffer_;

            std::vector<std::thread> workers_;
            std::mutex mutex_;
            std::condition_variable cond_var_;

            // ============

            std::atomic_bool is_need_running_{false};
            std::unique_ptr<std::thread> general_thread_;
        };

        Client::ClientImpl::ClientImpl() {}

        Client::ClientImpl::~ClientImpl()
        {
            if (is_started())
                stop();
        }

        bool Client::ClientImpl::start(const Config &config)
        {
            if (is_running())
            {
                LOG(INFO) << "Client is already running";
                return;
            }

            LOG(INFO) << "Starting...";

            return true;
        }

        void Client::ClientImpl::stop()
        {
            LOG(INFO) << "Stopped";
        }

        bool Client::ClientImpl::is_running() const
        {
            return true;
        }

        void Client::ClientImpl::run_general_thread() {}

        // ======================

        // bool Client::ClientImpl::start(const Config &config)
        // {
        //     if (is_started())
        //     {
        //         LOG(WARNING) << "Client is already started";
        //         return false;
        //     }

        //     LOG(INFO) << "Starting...";

        //     callbacks_ = config.callbacks_;

        //     io_context_.reset(new boost::asio::io_context(/* number of threads */));
        //     if (!io_context_)
        //     {
        //         LOG(ERROR) << "Can't create io_context";
        //         stop();
        //         return false;
        //     }

        //     resolver_.reset(new boost::asio::ip::tcp::resolver(*io_context_));
        //     if (!resolver_)
        //     {
        //         LOG(ERROR) << "Can't create resolver";
        //         stop();
        //         return false;
        //     }

        //     websocket_stream_.reset(new boost::beast::websocket::stream<boost::beast::tcp_stream>(*io_context_));
        //     if (!websocket_stream_)
        //     {
        //         LOG(ERROR) << "Can't create websocket_stream";
        //         stop();
        //         return false;
        //     }

        //     resolve(config);

        //     const int kWorkersNumber = 1;
        //     if (kWorkersNumber < 1)
        //     {
        //         LOG(ERROR) << "Number of available cores in too small";
        //         stop();
        //         return false;
        //     }

        //     LOG(INFO) << "Starting " << kWorkersNumber << " worker-threads...";

        //     workers_.reserve(kWorkersNumber);

        //     for (int thread_i = 0; thread_i < kWorkersNumber; ++thread_i)
        //     {
        //         workers_.emplace_back(
        //             [&]
        //             {
        //                 {
        //                     const std::lock_guard<std::mutex> lock(mutex_);
        //                     LOG(INFO) << "Starting worker [" << std::this_thread::get_id() << "]";
        //                 }

        //                 io_context_->run();
        //             });
        //     }

        //     return true;
        // }

        // void Client::ClientImpl::stop()
        // {
        //     if (!is_started())
        //     {
        //         LOG(WARNING) << "Client is already stopped";
        //         return;
        //     }

        //     LOG(INFO) << "Stopping...";

        //     if (!io_context_)
        //     {
        //         LOG(INFO) << "Stopped";
        //         return;
        //     }
        //     io_context_->stop();

        //     int worker_i = 0;
        //     for (auto &worker : workers_)
        //     {
        //         LOG(INFO) << "Worker(" << worker_i << ") stopping...";
        //         worker.join();
        //         LOG(INFO) << "Worker(" << worker_i++ << ") stopped";
        //     }
        //     workers_.clear();

        //     if (websocket_stream_)
        //     {
        //         if (websocket_stream_->is_open())
        //             websocket_stream_->close(boost::beast::websocket::close_code::normal);

        //         websocket_stream_.reset();
        //     }

        //     resolver_.reset();
        //     io_context_.reset();

        //     LOG(INFO) << "Stopped";
        // }

        bool Client::ClientImpl::send(const std::string &data)
        {
            if (!is_started())
            {
                LOG(ERROR) << "Server is not started";
                return false;
            }

            websocket_stream_->async_write(
                boost::asio::buffer(data),
                boost::bind(&Client::ClientImpl::on_send,
                            this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));

            return true;
        }

        bool Client::ClientImpl::is_started() const
        {
            if (!io_context_)
                return false;

            if (!resolver_)
                return false;

            if (!websocket_stream_)
                return false;

            if (workers_.empty())
                return false;

            return true;
        }

        void Client::ClientImpl::resolve(const Config &config, const bool is_reconnecting)
        {
            if (is_reconnecting)
            {
                LOG(INFO) << "Sleeping...";
                std::this_thread::sleep_for(std::chrono::seconds(config.reconnect_timeout_sec_));
            }

            resolver_->async_resolve(
                config.host_.c_str(), std::to_string(config.port_),
                boost::bind(&Client::ClientImpl::on_resolve,
                            this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::results,
                            config));
        }

        void Client::ClientImpl::on_resolve(boost::beast::error_code error_code,
                                            boost::asio::ip::tcp::resolver::results_type results,
                                            const Config &config)
        {
            LOG(INFO) << "Resolving...";

            if (error_code)
            {
                LOG(ERROR) << "Error " << error_code << " " << error_code.message();
                return;
            }

            // Set the timeout for the operation
            boost::beast::get_lowest_layer(*websocket_stream_).expires_after(std::chrono::seconds(5));
            // Make the connection on the IP address we get from a lookup
            boost::beast::get_lowest_layer(*websocket_stream_).async_connect(results, boost::bind(&Client::ClientImpl::on_connect, this, boost::asio::placeholders::error, boost::asio::placeholders::endpoint, config));
        }

        void Client::ClientImpl::on_connect(boost::beast::error_code error_code,
                                            boost::asio::ip::tcp::resolver::results_type::endpoint_type endpoint,
                                            const Config &config)
        {
            LOG(INFO) << "Connecting...";

            if (error_code)
            {
                LOG(ERROR) << "Error " << error_code << " " << error_code.message();
                resolve(config, kReconnecting);
                return;
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
                boost::bind(&Client::ClientImpl::on_handshake,
                            this,
                            boost::asio::placeholders::error,
                            config));
        }

        void Client::ClientImpl::on_handshake(boost::beast::error_code error_code,
                                              const Config &config)
        {
            if (error_code)
            {
                LOG(ERROR) << "Error " << error_code << " " << error_code.message();
                return;
            }

            LOG(INFO) << "Connection established";

            listen(config);
            // callbacks_.on_start_();
        }

        void Client::ClientImpl::on_send(boost::beast::error_code error_code,
                                         std::size_t bytes_transferred)
        {
            if (error_code)
            {
                LOG(ERROR) << "Error " << error_code << " " << error_code.message();
                return;
            }

            LOG(INFO) << "Sent " << bytes_transferred << " bytes";
        }

        void Client::ClientImpl::listen(const Config &config)
        {
            websocket_stream_->async_read(
                buffer_,
                boost::bind(
                    &Client::ClientImpl::on_receive,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred,
                    config));
        }

        void Client::ClientImpl::on_receive(boost::beast::error_code error_code,
                                            std::size_t bytes_transferred,
                                            const Config &config)
        {
            if (error_code)
            {
                LOG(ERROR) << "Error " << error_code << " " << error_code.message();
                close(config, kReconnecting);
                return;
            }

            callbacks_.process_receiving_(boost::beast::buffers_to_string(buffer_.data()));
            buffer_.clear();
            listen(config);
        }

        void Client::ClientImpl::close(const Config &config,
                                       bool is_reconnecting)
        {
            LOG(INFO) << "Closing...";
            LOG(INFO) << boost::beast::make_printable(buffer_.data());
            buffer_.clear();

            stop();

            if (is_reconnecting == kReconnecting)
                start(config);
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

        bool Client::is_running() const
        {
            if (!client_impl_)
            {
                LOG(ERROR) << "Implementation is not created";
                return;
            }

            return client_impl_->is_running();
        }

        bool Client::send(const std::string &data)
        {
            LOG(INFO) << "Sending...";

            if (!client_impl_)
            {
                static const std::string kErrorText("Implementation is not created");
                LOG(ERROR) << kErrorText;
                throw std::runtime_error(kErrorText);
            }

            return client_impl_->send(data);
        }
    }
}
