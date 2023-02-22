cmake_minimum_required(VERSION 3.5)

set(MODULE_NAME config_handling)

add_library(${MODULE_NAME}
    ${MODULE_NAME}.cpp
    ${MODULE_NAME}.hpp
)
add_library(server::config_handling ALIAS ${MODULE_NAME})

target_link_libraries(${MODULE_NAME} 
    PRIVATE
    easylogging::easylogging
)
target_include_directories(${MODULE_NAME}
    PRIVATE 
    ${CMAKE_BINARY_DIR}/third_party/nlohmann_json/
)