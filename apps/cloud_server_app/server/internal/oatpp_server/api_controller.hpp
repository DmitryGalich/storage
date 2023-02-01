#pragma once

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class ServerApiController : public oatpp::web::server::api::ApiController
{
protected:
    ServerApiController(const std::shared_ptr<ObjectMapper> &object_mapper)
        : oatpp::web::server::api::ApiController(object_mapper) {}

public:
    static std::shared_ptr<ServerApiController> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>,
                                                                             object_mapper))
    {
        return std::shared_ptr<ServerApiController>(new ServerApiController(object_mapper));
    }

    // ENDPOINT_ASYNC("GET", "/", Root)
    // {
    //     ENDPOINT_ASYNC_INIT(Root)

    //     Action act() override
    //     {
    //         auto dto = HelloDto::createShared();
    //         dto->message = "Hello Async!";
    //         dto->server = Header::Value::SERVER;
    //         dto->userAgent = request->getHeader(Header::USER_AGENT);
    //         return _return(controller->createDtoResponse(Status::CODE_200, dto));
    //     }
    // }
};

#include OATPP_CODEGEN_END(ApiController)
