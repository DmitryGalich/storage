cmake_minimum_required(VERSION 3.5)

find_package(oatpp              1.3.0 REQUIRED)
find_package(oatpp-websocket    1.3.0 REQUIRED)

set(MODULE_NAME oatpp_client)

add_library(${MODULE_NAME}
    oatpp_client.cpp
    oatpp_client.h
    api_holder.hpp
    api_model.hpp
)
add_library(cloud::internal::client_oatpp ALIAS ${MODULE_NAME})
target_link_libraries(${MODULE_NAME} 
    PRIVATE
        easylogging::easylogging
        oatpp::oatpp
        oatpp::oatpp-websocket
        cloud::internal::abstract_client
)
