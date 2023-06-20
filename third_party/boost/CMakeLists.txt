set(BOOST_LIB_NAME boost)
set(BOOST_LIB_DIR_NAME ${BOOST_LIB_NAME}-src)
set(BOOST_LIB_BUILD_NAME ${BOOST_LIB_NAME}-build)

include(FetchContent)
FetchContent_Populate(
  ${BOOST_LIB_NAME}
  URL        https://boostorg.jfrog.io/artifactory/main/release/1.82.0/source/boost_1_82_0.tar.gz
  SOURCE_DIR ${BOOST_LIB_DIR_NAME}
)

message("Installing htop...")

execute_process(
  COMMAND sudo apt-get install htop
)

message("Running bootstrap script to build...")

execute_process(
  COMMAND ./${BOOST_LIB_DIR_NAME}/bootstrap.sh
  WORKING_DIRECTORY ${BOOST_LIB_DIR_NAME}
)

message("Installing...")

execute_process(
  COMMAND sudo ./${BOOST_LIB_DIR_NAME}/b2 install
  WORKING_DIRECTORY ${BOOST_LIB_DIR_NAME}
)
