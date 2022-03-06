cmake_minimum_required(VERSION 3.16)

project(lib_logger
    LANGUAGES CXX
    VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -pedantic -Werror)

add_library(${PROJECT_NAME}
    src/logger.cpp
    include/logger.h)
target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(${PROJECT_NAME}
    PUBLIC
    -lpthread)

set(TESTS_TARGET ${PROJECT_NAME}_tests)
find_package(GTest 1.10 REQUIRED)
add_executable(${TESTS_TARGET} tests/logger_test.cpp)
target_link_libraries(${TESTS_TARGET}
    PRIVATE
    ${PROJECT_NAME}
    GTest::GTest)
target_include_directories(${TESTS_TARGET}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include)
