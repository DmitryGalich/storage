configure_file(CMakeLists.txt.in easyloggingpp_download/CMakeLists.txt)

# Download source code
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/easyloggingpp_download)
if(result)
  message(FATAL_ERROR "CMake step for easyloggingpp failed: ${result}")
endif()

# Build
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/easyloggingpp_download)
if(result)
  message(FATAL_ERROR "Build step for easyloggingpp failed: ${result}")
endif()

add_library(easyloggingpp 
  SHARED
    ${CMAKE_CURRENT_BINARY_DIR}/easyloggingpp_src/src/easylogging++.cc
    ${CMAKE_CURRENT_BINARY_DIR}/easyloggingpp_src/src/easylogging++.h
)
add_library(easylogging::easylogging ALIAS easyloggingpp)
