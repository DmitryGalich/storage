#pragma once

#include <fstream>

#include "oatpp-websocket/Handshaker.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/network/ConnectionHandler.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "../../../configs/cmake_config.h"

#include OATPP_CODEGEN_BEGIN(ApiController)

class ServerApiController : public oatpp::web::server::api::ApiController
{
private:
  typedef ServerApiController __ControllerType;

private:
  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, websocket_connection_handler, "websocket");

public:
  ServerApiController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, api_object_mapper))
      : oatpp::web::server::api::ApiController(api_object_mapper)
  {
  }

public:
  ENDPOINT_ASYNC("GET", "/", Root){
      ENDPOINT_ASYNC_INIT(Root)
          Action act() override{

              static const std::string kErrorHtmlFileContent("<!DOCTYPE html> <html> <body style=\"background-color:gray\"> <h2> Error of loading html-file </h2> </body> </html>");
  static const std::string kFilename(CMAKE_CURRENT_SOURCE_DIR + std::string{"/web/index.html"});

  std::ifstream file{kFilename};
  if (!file.is_open())
  {
    LOG(ERROR) << "Can't open file by path: " << kFilename;
    return _return(controller->createResponse(Status::CODE_404, kErrorHtmlFileContent));
  }

  std::ostringstream string_stream;
  string_stream << file.rdbuf();
  const std::string content = string_stream.str();
  return _return(controller->createResponse(Status::CODE_200, content));
}
}
;

ENDPOINT_ASYNC("GET", "ws", WS){
    ENDPOINT_ASYNC_INIT(WS)
        Action act() override{

            LOG(INFO) << "KEK";

auto response = oatpp::websocket::Handshaker::serversideHandshake(
    request->getHeaders(), controller->websocket_connection_handler);
return _return(response);
}
}
;
}
;

#include OATPP_CODEGEN_END(ApiController)
