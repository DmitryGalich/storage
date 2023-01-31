cmake_minimum_required(VERSION 3.5)

set(MODULE_NAME client)

add_subdirectory(internal)

add_library(${MODULE_NAME}
    ${MODULE_NAME}.cpp
    ${MODULE_NAME}.h
)
add_library(cloud::client ALIAS ${MODULE_NAME})

target_link_libraries(${MODULE_NAME}
    PRIVATE
        easylogging::easylogging
        cloud::internal::client_fabric
        cloud::internal::abstract_client 
)