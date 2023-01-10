cmake_minimum_required(VERSION 3.5)

project(cloud_server_app VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -pedantic -Werror)

configure_file(cmake_config.h.cmake ${CMAKE_CURRENT_LIST_DIR}/cmake_config.h @ONLY)

add_executable(${PROJECT_NAME} ${PROJECT_NAME}_console_main.cpp)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CMAKE_BINARY_DIR}/third_party/easylogging/easyloggingpp_src/src/
)

target_link_libraries(${PROJECT_NAME} 
    easylogging::easylogging
)

