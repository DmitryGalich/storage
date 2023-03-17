cmake_minimum_required(VERSION 3.5)

project(network_module 
        VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Boost =======================================
# find_package(Boost REQUIRED COMPONENTS system)
# include_directories(${Boost_INCLUDE_DIRS})
# link_directories(${Boost_LIBRARY_DIRS})

# file(GLOB_RECURSE BEAST_FILES
#   ${Boost_INCLUDE_DIRS}/boost/beast/*.hpp
#   ${Boost_INCLUDE_DIRS}/boost/beast/*.ipp)

# source_group(TREE ${Boost_INCLUDE_DIRS}/boost/beast PREFIX beast FILES ${BEAST_FILES})

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)

find_package(Boost COMPONENTS system)
IF(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
ENDIF(Boost_FOUND)

# =============================================

add_library(${PROJECT_NAME} 
    STATIC 
        src/${PROJECT_NAME}.cpp
)
target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        boost_system
)

# Testing ====================================

enable_testing()

include_directories(
    ${CMAKE_BINARY_DIR}/third_party/easylogging/easyloggingpp-src/src/)

set(PROJECT_NAME_TESTS ${PROJECT_NAME}_tests)
add_executable(${PROJECT_NAME_TESTS} 
    tests/${PROJECT_NAME_TESTS}.cpp)
target_link_libraries(${PROJECT_NAME_TESTS}    
    ${PROJECT_NAME}
    GTest::GTest 
    GTest::Main
    easylogging::easylogging
)

add_test(test_all ${PROJECT_NAME_TESTS})