cmake_minimum_required(VERSION 3.5)

include_directories(
    ${CMAKE_BINARY_DIR}/third_party/easylogging/easyloggingpp-src/src/)

if(NOT WIN32)
  find_package(Threads)
endif()

# Boost =======================================

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)

find_package(Boost COMPONENTS system)
IF(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
ENDIF(Boost_FOUND)

# =============================================

set(MODULE_NAME network_module)

add_library(${MODULE_NAME}
    ${MODULE_NAME}.hpp

    # client/client.cpp

    server/server.cpp
)
add_library(modules::network ALIAS ${MODULE_NAME})
target_link_libraries(${MODULE_NAME}
    PRIVATE
        easylogging::easylogging
)
target_include_directories(${MODULE_NAME}
    PRIVATE 
        ${CMAKE_BINARY_DIR}/third_party/nlohmann_json/
)

# Testing ======================================

enable_testing()

set(MODULE_NAME_TESTS ${MODULE_NAME}_tests)
add_executable(${MODULE_NAME_TESTS} 
    tests/${MODULE_NAME_TESTS}.cpp)
target_link_libraries(${MODULE_NAME_TESTS}    
    ${MODULE_NAME}
    GTest::GTest 
    GTest::Main
)

add_test(test_all ${MODULE_NAME_TESTS})