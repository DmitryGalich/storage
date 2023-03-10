cmake_minimum_required(VERSION 3.5)

add_subdirectory(internal)

set(MODULE_NAME server)

add_library(${MODULE_NAME}
    ${MODULE_NAME}.cpp
    ${MODULE_NAME}.hpp
)
add_library(server::server ALIAS ${MODULE_NAME})

target_link_libraries(${MODULE_NAME} 
PRIVATE
    easylogging::easylogging
    server::internal::abstract_server
    server::internal::server_fabric
)
