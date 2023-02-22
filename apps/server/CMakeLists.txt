cmake_minimum_required(VERSION 3.5)

project(server VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(oatpp              1.3.0 REQUIRED)
find_package(oatpp-websocket    1.3.0 REQUIRED)

configure_file(${CMAKE_CURRENT_LIST_DIR}/configs/cmake_config.h.cmake ${CMAKE_CURRENT_LIST_DIR}/configs/cmake_config.h @ONLY)

include_directories(${CMAKE_BINARY_DIR}/third_party/easylogging/easyloggingpp-src/src/)

add_executable(${PROJECT_NAME} 
    ${PROJECT_NAME}_console_main.cpp
    network_components.hpp
    WSListener.hpp
    WSListener.cpp
)
target_link_libraries(${PROJECT_NAME} 
    easylogging::easylogging
    oatpp::oatpp
    oatpp::oatpp-websocket
)
