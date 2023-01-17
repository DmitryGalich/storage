cmake_minimum_required(VERSION 3.5)

project(crypto_module 
        VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} 
    SHARED 
        src/${PROJECT_NAME}.cpp
)
target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/third_party/tiny_AES_c/tiny_AES_c
)

enable_testing()

set(PROJECT_NAME_TESTS ${PROJECT_NAME}_tests)
add_executable(${PROJECT_NAME_TESTS} 
    tests/${PROJECT_NAME_TESTS}.cpp)
target_link_libraries(${PROJECT_NAME_TESTS}    
    ${PROJECT_NAME}
    GTest::GTest 
    GTest::Main
)

add_test(test_all ${PROJECT_NAME_TESTS})
