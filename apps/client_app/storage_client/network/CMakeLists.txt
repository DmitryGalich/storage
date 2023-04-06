cmake_minimum_required(VERSION 3.5)

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
    ${MODULE_NAME}.cpp
    ${MODULE_NAME}.hpp
    http_session.hpp
    http_session.cpp    
    websocket_session.hpp
    websocket_session.cpp
    sessions_manager.hpp
    sessions_manager.cpp
)
add_library(storage::server::network ALIAS ${MODULE_NAME})

target_link_libraries(${MODULE_NAME}
    PRIVATE
        easylogging::easylogging
        boost_system
        Threads::Threads
)