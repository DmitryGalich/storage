cmake_minimum_required(VERSION 3.16)

project(tinyaes C ASM)

add_library(${PROJECT_NAME} aes.c aes.hpp aes.h)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
