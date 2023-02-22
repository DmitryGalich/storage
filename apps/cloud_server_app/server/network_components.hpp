#pragma once

#include "oatpp/core/async/Executor.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/network/ConnectionProvider.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

#include "WSListener.hpp"

namespace server
{
  class NetworkComponents
  {
  public:
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor)
    ([this]
     { return std::make_shared<oatpp::async::Executor>(); }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)
    ([]
     { return oatpp::network::tcp::server::ConnectionProvider::createShared({"127.0.0.1", 8000, oatpp::network::Address::IP_4}); }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)
    ([]
     { return oatpp::web::server::HttpRouter::createShared(); }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)
    ("http", []
     {
      OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
      OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);
      return oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, executor); }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper)
    ([]
     { return oatpp::parser::json::mapping::ObjectMapper::createShared(); }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, websocketConnectionHandler)
    ("websocket", []
     {
      OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);
      auto connectionHandler = oatpp::websocket::AsyncConnectionHandler::createShared(executor);
      connectionHandler->setSocketInstanceListener(std::make_shared<WSInstanceListener>());
      return connectionHandler; }());
  };
}