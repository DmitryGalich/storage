cmake_minimum_required(VERSION 3.5)

project(cloud_server_app VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} ${PROJECT_NAME}_main.cpp)

configure_file(cmake_config.h.cmake ${CMAKE_CURRENT_LIST_DIR}/cmake_config.h @ONLY)