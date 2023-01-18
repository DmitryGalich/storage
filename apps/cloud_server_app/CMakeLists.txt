cmake_minimum_required(VERSION 3.5)

project(cloud_server_app VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -pedantic -Werror)

configure_file(${CMAKE_CURRENT_LIST_DIR}/configs/cmake_config.h.cmake ${CMAKE_CURRENT_LIST_DIR}/configs/cmake_config.h @ONLY)

include_directories(${CMAKE_BINARY_DIR}/third_party/easylogging/easyloggingpp-src/src/)

add_subdirectory(server)

add_executable(${PROJECT_NAME} 
    ${PROJECT_NAME}_console_main.cpp
)
target_link_libraries(${PROJECT_NAME} 
    easylogging::easylogging
    cloud::server
)

