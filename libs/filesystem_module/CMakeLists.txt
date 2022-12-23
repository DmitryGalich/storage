cmake_minimum_required(VERSION 3.22)

project(filesystem_module 
        VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} 
    STATIC 
        src/${PROJECT_NAME}.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)