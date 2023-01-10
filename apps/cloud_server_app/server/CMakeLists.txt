cmake_minimum_required(VERSION 3.5)

add_library(server
    server.cpp
    server.h
)
add_library(cloud::server ALIAS server)

target_link_libraries(server
    easylogging::easylogging
)