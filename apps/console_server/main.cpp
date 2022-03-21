#include <iostream>

#include "cmake_config.h"

#include "logger.h"

int main() {
  const std::string kVersion(
      "(" + std::to_string(console_server_VERSION_MINOR) + "." +
      std::to_string(console_server_VERSION_MINOR) + "." +
      std::to_string(console_server_VERSION_PATCH) + ")");

  log::info(console_server_PROJECT_NAME + kVersion, __FUNCTION__);

  return 0;
}
