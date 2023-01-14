include(FetchContent)
FetchContent_Populate(
  tiny_AES_c
  URL        https://github.com/kokke/tiny-AES-c/archive/refs/tags/v1.0.0.tar.gz
  SOURCE_DIR tiny_AES_c
)

add_library(tiny_AES_c 
  SHARED
    ${CMAKE_CURRENT_BINARY_DIR}/tiny_AES_c/aes.h
    ${CMAKE_CURRENT_BINARY_DIR}/tiny_AES_c/aes.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/tiny_AES_c/aes.c
)
add_library(tiny_AES_c::tiny_AES_c ALIAS tiny_AES_c)