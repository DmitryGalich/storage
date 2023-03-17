cmake_minimum_required(VERSION 3.5)

set(MODULE_NAME storage_server)

add_library(${MODULE_NAME}
    ${MODULE_NAME}.cpp
    ${MODULE_NAME}.hpp
)
add_library(storage::server ALIAS ${MODULE_NAME})

target_link_libraries(${MODULE_NAME}
    PRIVATE
        easylogging::easylogging
)