cmake_minimum_required(VERSION 3.5)

add_subdirectory(config_handling)

find_package(oatpp              1.3.0 REQUIRED)
find_package(oatpp-websocket    1.3.0 REQUIRED)

set(MODULE_NAME server)

add_library(${MODULE_NAME}
    ${MODULE_NAME}.cpp
    ${MODULE_NAME}.hpp
)
add_library(server::server ALIAS ${MODULE_NAME})

target_link_libraries(${MODULE_NAME} 
PRIVATE
    easylogging::easylogging
    oatpp::oatpp
    oatpp::oatpp-websocket
    server::config_handling
)
