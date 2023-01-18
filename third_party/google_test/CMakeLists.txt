set(GTEST_LIB_NAME googletest)
set(GTEST_LIB_DIR_NAME ${GTEST_LIB_NAME}-src)
set(GTEST_LIB_BUILD_NAME ${GTEST_LIB_NAME}-build)

include(FetchContent)
FetchContent_Populate(
  ${GTEST_LIB_NAME}
  URL        https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz
  SOURCE_DIR ${GTEST_LIB_DIR_NAME}
)

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/${GTEST_LIB_DIR_NAME}
                 ${CMAKE_CURRENT_BINARY_DIR}/${GTEST_LIB_BUILD_NAME})

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# Prevent installation of GTest with your project
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(INSTALL_GMOCK OFF CACHE BOOL "" FORCE)

# Add aliases for GTest and GMock libraries
if(NOT TARGET GTest::GTest)
    add_library(GTest::GTest ALIAS gtest)
    add_library(GTest::Main ALIAS gtest_main)
endif()

if(NOT TARGET GTest::GMock)
    add_library(GMock::GMock ALIAS gmock)
    add_library(GMock::Main ALIAS gmock_main)
endif()