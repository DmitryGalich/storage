cmake_minimum_required(VERSION 3.5)

project(client
        VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Logger ======================================

include_directories(
    ${CMAKE_BINARY_DIR}/third_party/easylogging/easyloggingpp-src/src/)

# =============================================

configure_file(${CMAKE_CURRENT_LIST_DIR}/configs/cmake_config.h.cmake ${CMAKE_CURRENT_LIST_DIR}/configs/cmake_config.h @ONLY)

# add_subdirectory(storage_server)

add_executable(${PROJECT_NAME}
    ${PROJECT_NAME}_main.cpp
)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        easylogging::easylogging
        # storage::server
)
