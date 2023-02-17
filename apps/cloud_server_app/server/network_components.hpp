#pragma once

#include "oatpp/core/async/Executor.hpp"
#include "oatpp/core/macro/component.hpp"

#include "config_handling/config_handling.hpp"

namespace server
{
  class NetworkComponents
  {
  public:
    NetworkComponents() = delete;
    NetworkComponents(const server::config::Config &config)
        : kConfig_(config) {}

  private:
    const server::config::Config &kConfig_;

  public:
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor)
    ([this]
     { return std::make_shared<oatpp::async::Executor>(
           kConfig_.executor_data_processing_threads_,
           kConfig_.executor_io_threads_,
           kConfig_.executor_timer_threads_); }());

    // OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)
    // ([]
    //  { return oatpp::network::tcp::server::ConnectionProvider::createShared({"127.0.0.1", 8000, oatpp::network::Address::IP_4}); }());

    // OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)
    // ([]
    //  { return oatpp::web::server::HttpRouter::createShared(); }());

    // OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)
    // ("http", []
    //  {
    //   OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
    //   OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);
    //   return oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, executor); }());

    // OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper)
    // ([]
    //  { return oatpp::parser::json::mapping::ObjectMapper::createShared(); }());

    // OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, websocketConnectionHandler)
    // ("websocket", []
    //  {
    //   OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);
    //   auto connectionHandler = oatpp::websocket::AsyncConnectionHandler::createShared(executor);
    //   connectionHandler->setSocketInstanceListener(std::make_shared<WSInstanceListener>());
    //   return connectionHandler; }());
  };
}