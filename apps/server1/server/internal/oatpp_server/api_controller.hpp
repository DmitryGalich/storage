#pragma once

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "api_model.hpp"

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

    /**
     *  Hello World endpoint Coroutine mapped to the "/" (root)
     */
    ENDPOINT_ASYNC("GET", "hello", Root){
        ENDPOINT_ASYNC_INIT(Root)

        /**
         *  Coroutine entrypoint act()
         *  returns Action (what to do next)
         */
        Action act() override{

            auto dto = HelloDto::createShared();
    dto->message = "Hello Async!";

    dto->server = Header::Value::SERVER;
    dto->userAgent = request->getHeader(Header::USER_AGENT);
    return _return(controller->createDtoResponse(Status::CODE_200, dto));
}
}
;

/**
 *  Echo body endpoint Coroutine. Mapped to "/body/string".
 *  Returns body received in the request
 */
ENDPOINT_ASYNC("GET", "/body/string", EchoStringBody){

    ENDPOINT_ASYNC_INIT(EchoStringBody)

        Action act() override{
            /* return Action to start child coroutine to read body */
            return request->readBodyToStringAsync().callbackTo(&EchoStringBody::returnResponse);
}

Action returnResponse(const oatpp::String &body)
{
    /* return Action to return created OutgoingResponse */
    return _return(controller->createResponse(Status::CODE_200, body));
}
}
;

/**
 *  Echo body endpoint Coroutine. Mapped to "/body/dto".
 *  Deserialize DTO reveived, and return same DTO
 *  Returns body as MessageDto received in the request
 */
ENDPOINT_ASYNC("GET", "/body/dto", EchoDtoBody){

    ENDPOINT_ASYNC_INIT(EchoDtoBody)

        Action act() override{
            return request->readBodyToDtoAsync<oatpp::Object<MessageDto>>(controller->getDefaultObjectMapper()).callbackTo(&EchoDtoBody::returnResponse);
}

Action returnResponse(const oatpp::Object<MessageDto> &body)
{
    return _return(controller->createDtoResponse(Status::CODE_200, body));
}
}
;
}
;

#include OATPP_CODEGEN_END(ApiController)
