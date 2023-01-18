include(FetchContent)
FetchContent_Populate(
  easyloggingpp
  URL        https://github.com/amrayn/easyloggingpp/archive/refs/tags/v9.97.0.tar.gz
  SOURCE_DIR easyloggingpp-src
)

add_library(easyloggingpp 
  SHARED
    ${CMAKE_CURRENT_BINARY_DIR}/easyloggingpp-src/src/easylogging++.cc
    ${CMAKE_CURRENT_BINARY_DIR}/easyloggingpp-src/src/easylogging++.h
)
add_library(easylogging::easylogging ALIAS easyloggingpp)
