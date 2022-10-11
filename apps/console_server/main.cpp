#include <iostream>

#include "cmake_config.h"

#include "logger.h"
#include "network.h"

int parse_arguments(const int argc, char **argv, std::string &logger_path,
                    std::string &ip, uint32_t &port)
{
  if (argc < 4)
  {
    std::cerr << "Not enough arguments. Need logger_folder_path, ip, port"
              << std::endl;
    return -1;
  }
  else if (argc == 4)
  {
    if (!std::filesystem::is_directory(argv[1]))
    {
      std::cerr << "Unexisting path" << std::endl;
      return -1;
    }

    logger_path = argv[1];
    ip = argv[2];
    port = strtoul(argv[3], NULL, 0);
  }
  else
  {
    std::cerr << "Incorrect arguments" << std::endl;
    return -1;
  }

  return 0;
}

void check_user_input()
{
  char input_symbol = 0x00;
  do
  {
    std::this_thread::sleep_for(std::chrono_literals::operator""ms(500));

    std::cin >> input_symbol;

    if (input_symbol == 'q')
      break;
    else
      std::cout << "Press \'q\' to stop" << std::endl;

  } while (true);
}

int main(int argc, char **argv)
{
  std::string logger_path;
  std::string ip;
  uint32_t port;

  int status = parse_arguments(argc, argv, logger_path, ip, port);
  if (status != 0)
    return status;

  const std::string kApplicationName(
      console_server_PROJECT_NAME + std::string("_") +
      std::to_string(console_server_VERSION_MAJOR) + std::string("_") +
      std::to_string(console_server_VERSION_MINOR) + std::string("_") +
      std::to_string(console_server_VERSION_PATCH));

  logger::set_application_name(kApplicationName);
  logger::set_path(logger_path);
  logger::info(kApplicationName + " started");

  network::Server server;
  server.start(ip, port);

  check_user_input();

  server.stop();

  logger::info(kApplicationName + " stopped");

  return 0;
}
