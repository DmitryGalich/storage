#pragma once

#include "api_model.hpp"

#include "oatpp/web/client/ApiClient.hpp"
#include "oatpp/core/macro/codegen.hpp"

class ClientApiHolder : public oatpp::web::client::ApiClient
{
#include OATPP_CODEGEN_BEGIN(ApiClient)

    API_CLIENT_INIT(ClientApiHolder)

    API_CALL_ASYNC("GET", "{parameter}", doGetAsync, PATH(String, parameter))

#include OATPP_CODEGEN_END(ApiClient)
};