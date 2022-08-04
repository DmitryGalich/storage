#include <iostream>

#include "cmake_config.h"

#include "logger.h"

int main()
{
  const std::string kTitle("console_app");
  const std::string kVersion("(" + std::to_string(console_app_VERSION_MINOR) +
                             "." + std::to_string(console_app_VERSION_MINOR) +
                             "." + std::to_string(console_app_VERSION_PATCH) +
                             ")");

  logger::info(kTitle + kVersion, __FUNCTION__);

  return 0;
}
