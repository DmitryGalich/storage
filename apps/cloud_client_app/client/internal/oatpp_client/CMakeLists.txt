cmake_minimum_required(VERSION 3.5)

set(MODULE_NAME oatpp_client)

add_library(${MODULE_NAME}
    oatpp_client.cpp
    oatpp_client.h
    api_client.hpp
    api_model.hpp
    async_executor.hpp
    async_executor.cpp
)
add_library(cloud::internal::client_oatpp ALIAS ${MODULE_NAME})
target_include_directories(${MODULE_NAME}
    PRIVATE 
        ${CMAKE_BINARY_DIR}/third_party/oatpp/oatpp-src/src/oatpp/
)
target_link_libraries(${MODULE_NAME} 
    PRIVATE
        easylogging::easylogging
        oatpp::oatpp
        cloud::internal::abstract_client
)
