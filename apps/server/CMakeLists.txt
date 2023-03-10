# cmake_minimum_required(VERSION 3.1)

# set(project_name async-websocket-server) ## rename your project here

# project(${project_name})

# set(CMAKE_CXX_STANDARD 11)


# add_library(${project_name}-lib
#         src/AppComponent.hpp
#         src/controller/MyController.hpp
#         src/websocket/WSListener.cpp
#         src/websocket/WSListener.hpp
# )

# find_package(oatpp              1.3.0 REQUIRED)
# find_package(oatpp-websocket    1.3.0 REQUIRED)

# target_link_libraries(${project_name}-lib
#         PUBLIC oatpp::oatpp
#         PUBLIC oatpp::oatpp-websocket
# )

# add_executable(${project_name}-exe
#         src/App.cpp
# )
# target_link_libraries(${project_name}-exe ${project_name}-lib)
# add_dependencies(${project_name}-exe ${project_name}-lib)

cmake_minimum_required(VERSION 3.5)

project(cloud_server_app VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file(${CMAKE_CURRENT_LIST_DIR}/configs/cmake_config.h.cmake ${CMAKE_CURRENT_LIST_DIR}/configs/cmake_config.h @ONLY)

include_directories(${CMAKE_BINARY_DIR}/third_party/easylogging/easyloggingpp-src/src/)

add_subdirectory(server)

add_executable(${PROJECT_NAME} 
    ${PROJECT_NAME}_console_main.cpp
)
target_link_libraries(${PROJECT_NAME} 
    easylogging::easylogging
    server::server
)
