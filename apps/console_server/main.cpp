#include <iostream>

#include "cmake_config.h"

#include "logger.h"

int main(int argc, char **argv) {
  std::string log_path;

  if (argc < 2) {
    std::cerr << "Not enough arguments. Need log folder path" << std::endl;
    return -1;
  } else if (argc == 2) {
    if (!std::filesystem::is_directory(argv[1])) {
      std::cerr << "Unexisting path" << std::endl;
      return -1;
    }

    log_path = argv[1];
  } else {
    std::cerr << "Incorrect arguments" << std::endl;
    return -1;
  }

  {
    const std::string kApplicationName(
        console_server_PROJECT_NAME + std::string("_") +
        std::to_string(console_server_VERSION_MAJOR) + std::string("_") +
        std::to_string(console_server_VERSION_MINOR) + std::string("_") +
        std::to_string(console_server_VERSION_PATCH));

    log::set_application_name(kApplicationName);
    log::set_path(log_path);
    log::info(kApplicationName);
  }

  return 0;
}
