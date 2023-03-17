cmake_minimum_required(VERSION 3.5)

set(MODULE_NAME oatpp_server)

add_library(${MODULE_NAME}
    ${MODULE_NAME}.cpp
    ${MODULE_NAME}.h
    api_controller.hpp
    api_model.hpp
)
add_library(cloud::internal::server_oatpp ALIAS ${MODULE_NAME})
target_include_directories(${MODULE_NAME}
    PRIVATE 
        ${CMAKE_BINARY_DIR}/third_party/oatpp/oatpp-src/src/oatpp/
)
target_link_libraries(${MODULE_NAME} 
    PRIVATE
        easylogging::easylogging
        oatpp::oatpp
        cloud::internal::abstract_server
)
