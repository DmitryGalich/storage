cmake_minimum_required(VERSION 3.5)

add_subdirectory(oatpp_client)

set(CLIENT_FABRIC_MODULE_NAME client_fabric)
add_library(${CLIENT_FABRIC_MODULE_NAME}
        ${CLIENT_FABRIC_MODULE_NAME}.h
        ${CLIENT_FABRIC_MODULE_NAME}.cpp
)
add_library(cloud::internal::client_fabric ALIAS ${CLIENT_FABRIC_MODULE_NAME})
target_link_libraries(${CLIENT_FABRIC_MODULE_NAME} 
    PUBLIC
        easylogging::easylogging
        cloud::internal::abstract_client
        cloud::internal::client_oatpp
)

set(ABSTRACT_CLIENT_MODULE_NAME abstract_client)
add_library(${ABSTRACT_CLIENT_MODULE_NAME} INTERFACE ${ABSTRACT_CLIENT_MODULE_NAME}.h)
add_library(cloud::internal::abstract_client ALIAS ${ABSTRACT_CLIENT_MODULE_NAME})
