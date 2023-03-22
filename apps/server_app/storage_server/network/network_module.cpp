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
    std::size_t
    request_count()
    {
        static std::size_t count = 0;
        return ++count;
    }

    std::time_t
    now()
    {
        return std::time(0);
    }
}

class http_connection : public std::enable_shared_from_this<http_connection>
{
public:
    http_connection() = delete;
    http_connection(
        tcp::socket socket)
        : socket_(std::move(socket))
    {
    }

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

    net::steady_timer deadline_{
        socket_.get_executor(), std::chrono::seconds(60)};

    void read_request()
    {
        auto self = shared_from_this();

        http::async_read(
            socket_,
            buffer_,
            request_,
            [self](beast::error_code ec,
                   std::size_t bytes_transferred)
            {
                boost::ignore_unused(bytes_transferred);
                if (!ec)
                    self->process_request();
            });
    }

    void process_request()
    {
        response_.version(request_.version());
        response_.keep_alive(false);

        switch (request_.method())
        {
        case http::verb::get:
            response_.result(http::status::ok);
            response_.set(http::field::server, "Beast");
            create_response();
            break;

        default:
            // We return responses indicating an error if
            // we do not recognize the request method.
            response_.result(http::status::bad_request);
            response_.set(http::field::content_type, "text/plain");
            beast::ostream(response_.body())
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
            response_.set(http::field::content_type, "text/html");
            beast::ostream(response_.body())
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
            response_.set(http::field::content_type, "text/html");
            beast::ostream(response_.body())
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
            response_.result(http::status::not_found);
            response_.set(http::field::content_type, "text/plain");
            beast::ostream(response_.body()) << "File not found\r\n";
        }
    }

    void write_response()
    {
        auto self = shared_from_this();

        response_.content_length(response_.body().size());

        http::async_write(
            socket_,
            response_,
            [self](beast::error_code ec, std::size_t)
            {
                self->socket_.shutdown(tcp::socket::shutdown_send, ec);
                self->deadline_.cancel();
            });
    }

    void check_deadline()
    {
        auto self = shared_from_this();

        deadline_.async_wait(
            [self](beast::error_code ec)
            {
                if (!ec)
                {
                    self->socket_.close(ec);
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

                io_context_.reset(new boost::asio::io_context(/* num of threads */));
                if (!io_context_)
                {
                    LOG(ERROR) << "Can't create io_context";
                    return false;
                }

                acceptor_.reset(new boost::asio::ip::tcp::acceptor(*io_context_, endpoint));
                if (!acceptor_)
                {
                    LOG(ERROR) << "Can't create acceptor";
                    return false;
                }

                socket_.reset(new boost::asio::ip::tcp::socket(*io_context_));
                if (!acceptor_)
                {
                    LOG(ERROR) << "Can't create acceptor";
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
                                                std::make_shared<http_connection>(std::move(*socket_))->start();
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