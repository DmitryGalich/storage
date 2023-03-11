cmake_minimum_required(VERSION 3.5)

find_package(oatpp              1.3.0 REQUIRED)
find_package(oatpp-websocket    1.3.0 REQUIRED)

set(MODULE_NAME oatpp_server)

add_library(${MODULE_NAME}
    ${MODULE_NAME}.cpp
    ${MODULE_NAME}.h
    # AppComponent.hpp
    # controller/MyController.hpp
    # websocket/WSListener.cpp
    # websocket/WSListener.hpp
)
add_library(server::internal::server_oatpp ALIAS ${MODULE_NAME})
target_include_directories(${MODULE_NAME}
    PRIVATE 
        ${CMAKE_BINARY_DIR}/third_party/oatpp/oatpp-src/src/oatpp/
)
target_link_libraries(${MODULE_NAME} 
    PRIVATE
        easylogging::easylogging
        server::internal::abstract_server
        oatpp::oatpp
        oatpp::oatpp-websocket
)
