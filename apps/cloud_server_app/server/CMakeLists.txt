cmake_minimum_required(VERSION 3.5)

set(MODULE_NAME server)

add_subdirectory(internal)

add_library(${MODULE_NAME}
    ${MODULE_NAME}.cpp
    ${MODULE_NAME}.h
)
add_library(cloud::server ALIAS ${MODULE_NAME})

target_link_libraries(${MODULE_NAME}
    PRIVATE
        easylogging::easylogging
        cloud::internal::server_fabric
        cloud::internal::abstract_server 
)