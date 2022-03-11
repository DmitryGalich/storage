cmake_minimum_required(VERSION 3.16)

project(lib_network
    LANGUAGES CXX
    VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -pedantic -Werror)

add_library(${PROJECT_NAME}
    src/network.cpp
    include/network.h)
target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include)

set(TESTS_TARGET ${PROJECT_NAME}_tests)
find_package(GTest 1.10 REQUIRED)
add_executable(${TESTS_TARGET} tests/network_test.cpp)
target_link_libraries(${TESTS_TARGET}
    PRIVATE
    ${PROJECT_NAME}
    GTest::GTest)
target_include_directories(${TESTS_TARGET}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include)
