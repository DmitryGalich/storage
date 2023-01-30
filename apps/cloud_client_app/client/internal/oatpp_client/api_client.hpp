#pragma once

#include "api_model.hpp"

#include "oatpp/web/client/ApiClient.hpp"
#include "oatpp/core/macro/codegen.hpp"

class ClientApiHolder : public oatpp::web::client::ApiClient
{
#include OATPP_CODEGEN_BEGIN(ApiClient)

    API_CLIENT_INIT(ClientApiHolder)

    // API_CALL_ASYNC("GET", "get", doGetAsync)
    // API_CALL_ASYNC("POST", "post", doPostAsync, BODY_STRING(String, body))
    // API_CALL_ASYNC("PUT", "put", doPutAsync, BODY_STRING(String, body))
    // API_CALL_ASYNC("PATCH", "patch", doPatchAsync, BODY_STRING(String, body))
    // API_CALL_ASYNC("DELETE", "delete", doDeleteAsync)

    // API_CALL_ASYNC("POST", "post", doPostWithDtoAsync, BODY_DTO(Object<RequestDto>, body))

    // API_CALL_ASYNC("GET", "anything/{parameter}", doGetAnythingAsync, PATH(String, parameter))
    // API_CALL_ASYNC("POST", "anything/{parameter}", doPostAnythingAsync, PATH(String, parameter), BODY_STRING(String, body))
    // API_CALL_ASYNC("PUT", "anything/{parameter}", doPutAnythingAsync, PATH(String, parameter), BODY_STRING(String, body))
    // API_CALL_ASYNC("PATCH", "anything/{parameter}", doPatchAnythingAsync, PATH(String, parameter), BODY_STRING(String, body))
    // API_CALL_ASYNC("DELETE", "anything/{parameter}", doDeleteAnythingAsync, PATH(String, parameter))

    //==========

    API_CALL_ASYNC("GET", "{parameter}", doGetAsync, PATH(String, parameter))

#include OATPP_CODEGEN_END(ApiClient)
};