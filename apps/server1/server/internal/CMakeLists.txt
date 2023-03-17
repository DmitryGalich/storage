cmake_minimum_required(VERSION 3.5)

set(ABSTRACT_SERVER_MODULE_NAME abstract_server)
add_library(${ABSTRACT_SERVER_MODULE_NAME} 
    INTERFACE 
        ${ABSTRACT_SERVER_MODULE_NAME}.h)
add_library(cloud::internal::abstract_server ALIAS ${ABSTRACT_SERVER_MODULE_NAME})

add_subdirectory(oatpp_server)

set(SERVER_FABRIC_MODULE_NAME server_fabric)
add_library(${SERVER_FABRIC_MODULE_NAME}
        ${SERVER_FABRIC_MODULE_NAME}.h
        ${SERVER_FABRIC_MODULE_NAME}.cpp
)
add_library(cloud::internal::server_fabric ALIAS ${SERVER_FABRIC_MODULE_NAME})
target_link_libraries(${SERVER_FABRIC_MODULE_NAME} 
    PRIVATE
        easylogging::easylogging
        cloud::internal::abstract_server
        cloud::internal::server_oatpp
)
target_include_directories(${SERVER_FABRIC_MODULE_NAME}
    PRIVATE 
        ${CMAKE_BINARY_DIR}/third_party/nlohmann_json/
)