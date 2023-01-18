cmake_minimum_required(VERSION 3.5)

add_library(client
    client.cpp
    client.h
)
add_library(cloud::client ALIAS client)

target_link_libraries(client
    easylogging::easylogging
)