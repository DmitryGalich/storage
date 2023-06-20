set(BOOST_LIB_NAME boost)
set(BOOST_LIB_DIR_NAME ${BOOST_LIB_NAME}-src)
set(BOOST_LIB_BUILD_NAME ${BOOST_LIB_NAME}-build)

include(FetchContent)
FetchContent_Populate(
  ${BOOST_LIB_NAME}
  URL        https://boostorg.jfrog.io/artifactory/main/release/1.82.0/source/boost_1_82_0.tar.gz
  SOURCE_DIR ${BOOST_LIB_DIR_NAME}
)

execute_process(
  COMMAND ./bootstrap.sh
  WORKING_DIRECTORY ${BOOST_LIB_DIR_NAME}
)

execute_process(
  COMMAND sudo ./b2 install
  WORKING_DIRECTORY ${BOOST_LIB_DIR_NAME}
)
