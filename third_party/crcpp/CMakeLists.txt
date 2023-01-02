configure_file(CMakeLists.txt.in crcpp_download/CMakeLists.txt)

# Download source code
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/crcpp_download)
if(result)
  message(FATAL_ERROR "CMake step for crcpp failed: ${result}")
endif()

# Build
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/crcpp_download)
if(result)
  message(FATAL_ERROR "Build step for crcpp failed: ${result}")
endif()

# Add lib directly to build
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/crcpp_src
                 ${CMAKE_CURRENT_BINARY_DIR}/crcpp_build)
