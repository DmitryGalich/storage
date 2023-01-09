cmake_minimum_required(VERSION 3.5)

project(logger_module 
        VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} tests/${PROJECT_NAME}_tests.cpp)

target_include_directories(${PROJECT_NAME}
    PUBLIC 
    ${CMAKE_BINARY_DIR}/third_party/easylogging/easyloggingpp_src/src/
)

target_link_libraries(${PROJECT_NAME} easylogging::easylogging)
