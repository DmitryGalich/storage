cmake_minimum_required(VERSION 3.16)

project(console_app
    LANGUAGES CXX
    VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -pedantic -Werror)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    lib_logger)

configure_file(cmake_config.h.cmake ${CMAKE_CURRENT_LIST_DIR}/cmake_config.h @ONLY)
