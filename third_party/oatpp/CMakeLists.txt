set(OATPP_LIB_NAME oatpp)
set(OATPP_LIB_DIR_NAME ${OATPP_LIB_NAME}-src)
set(OATPP_LIB_BUILD_NAME ${OATPP_LIB_NAME}-build)

include(FetchContent)
FetchContent_Populate(
  ${OATPP_LIB_NAME}
  URL        https://github.com/oatpp/oatpp/archive/refs/tags/1.3.0.tar.gz
  SOURCE_DIR ${OATPP_LIB_DIR_NAME}
)

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/${OATPP_LIB_DIR_NAME}
                 ${CMAKE_CURRENT_BINARY_DIR}/${OATPP_LIB_BUILD_NAME})

add_library(oatpp::oatpp ALIAS oatpp)