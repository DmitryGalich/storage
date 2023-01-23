cmake_minimum_required(VERSION 3.5)

set(MODULE_NAME client)

add_library(${MODULE_NAME}
    ${MODULE_NAME}.cpp
    ${MODULE_NAME}.h
    DemoApiClient.hpp
    DemoApiModels.hpp
    AsyncExample.hpp
    SimpleExample.hpp
)
add_library(cloud::client ALIAS ${MODULE_NAME})

target_include_directories(${MODULE_NAME}
    PUBLIC 
        ${CMAKE_BINARY_DIR}/third_party/oatpp/oatpp-src/src/oatpp/
)
target_link_libraries(${MODULE_NAME}
    easylogging::easylogging
    oatpp::oatpp
)