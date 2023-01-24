cmake_minimum_required(VERSION 3.5)

set(MODULE_NAME client)

add_subdirectory(internal)

add_library(${MODULE_NAME}
    client.cpp
    client.h
)
add_library(cloud::client ALIAS ${MODULE_NAME})

target_link_libraries(${MODULE_NAME}
    easylogging::easylogging
    cloud::internal::client_fabric
    # cloud::internal::abstract_client
)