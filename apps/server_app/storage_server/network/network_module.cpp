#include "network_module.hpp"

#include <thread>

#include "easylogging++.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>

#include "listener.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>

namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
namespace net = boost::asio;      // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

namespace my_program_state
{
    std::size_t request_count()
    {
        static std::size_t count = 0;
        return ++count;
    }

    std::time_t now()
    {
        return std::time(0);
    }
}

class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
public:
    HttpConnection() = delete;
    HttpConnection(tcp::socket socket)
        : socket_(std::move(socket))
    {
    }
    ~HttpConnection() = default;

    void start()
    {
        read_request();
        check_deadline();
    }

private:
    boost::asio::ip::tcp::socket socket_;
    boost::beast::flat_buffer buffer_{8192};
    boost::beast::http::request<boost::beast::http::dynamic_body> request_;
    boost::beast::http::response<boost::beast::http::dynamic_body> response_;

    boost::asio::steady_timer deadline_{socket_.get_executor(), std::chrono::seconds(60)};

    void read_request()
    {
        auto self = shared_from_this();

        boost::beast::http::async_read(
            socket_,
            buffer_,
            request_,
            [self](boost::beast::error_code error_code,
                   std::size_t bytes_transferred)
            {
                boost::ignore_unused(bytes_transferred);

                if (error_code)
                {
                    LOG(ERROR) << "async_read - (" << error_code.value() << ") " << error_code.message();
                }
                else
                {
                    self->process_request();
                }
            });
    }

    void process_request()
    {
        response_.version(request_.version());
        response_.keep_alive(false);

        switch (request_.method())
        {
        case boost::beast::http::verb::get:
            response_.result(boost::beast::http::status::ok);
            response_.set(boost::beast::http::field::server, "Beast");
            create_response();
            break;

        default:
            response_.result(boost::beast::http::status::bad_request);
            response_.set(boost::beast::http::field::content_type, "text/plain");
            boost::beast::ostream(response_.body())
                << "Invalid request-method '"
                << std::string(request_.method_string())
                << "'";
            break;
        }

        write_response();
    }

    void create_response()
    {
        if (request_.target() == "/count")
        {
            response_.set(boost::beast::http::field::content_type,
                          "text/html");

            boost::beast::ostream(response_.body())
                << "<html>\n"
                << "<head><title>Request count</title></head>\n"
                << "<body>\n"
                << "<h1>Request count</h1>\n"
                << "<p>There have been "
                << my_program_state::request_count()
                << " requests so far.</p>\n"
                << "</body>\n"
                << "</html>\n";
        }
        else if (request_.target() == "/time")
        {
            response_.set(boost::beast::http::field::content_type,
                          "text/html");
            boost::beast::ostream(response_.body())
                << "<html>\n"
                << "<head><title>Current time</title></head>\n"
                << "<body>\n"
                << "<h1>Current time</h1>\n"
                << "<p>The current time is "
                << my_program_state::now()
                << " seconds since the epoch.</p>\n"
                << "</body>\n"
                << "</html>\n";
        }
        else
        {
            response_.result(boost::beast::http::status::not_found);
            response_.set(boost::beast::http::field::content_type, "text/plain");
            boost::beast::ostream(response_.body()) << "File not found\r\n";
        }
    }

    void write_response()
    {
        auto self = shared_from_this();

        response_.content_length(response_.body().size());

        boost::beast::http::async_write(
            socket_,
            response_,
            [self](boost::beast::error_code error_code, std::size_t)
            {
                self->socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send, error_code);
                self->deadline_.cancel();
            });
    }

    void check_deadline()
    {
        auto self = shared_from_this();

        deadline_.async_wait(
            [self](beast::error_code error_code)
            {
                if (error_code)
                {
                    LOG(ERROR) << "async_read - (" << error_code.value() << ") " << error_code.message();
                }
                else
                {
                    self->socket_.close(error_code);
                }
            });
    }
};

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

                acceptor_->set_option(net::socket_base::reuse_address(true));
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
                                                LOG(ERROR) << "async_accept - (" << error_code.value() << ") " << error_code.message();
                                            }
                                            else
                                            {
                                                LOG(INFO) << "Creating new http connection...";
                                                std::make_shared<HttpConnection>(std::move(*socket_))->start();
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