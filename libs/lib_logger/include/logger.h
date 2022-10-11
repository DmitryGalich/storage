#pragma once

#include <filesystem>
#include <string>
#include <thread>

#define LOG_INFO(message) logger::info(message, __PRETTY_FUNCTION__)
#define LOG_WARNING(message) logger::warning(message, __PRETTY_FUNCTION__)
#define LOG_ERROR(message) logger::error(message, __PRETTY_FUNCTION__)

namespace logger
{
    void set_path(const std::filesystem::path &path);
    void clear_path();

    void set_application_name(const std::string &app_name);
    void clear_application_name();

    void info(const std::string &message, const std::string function_prefix = "");
    void warning(const std::string &message,
                 const std::string function_prefix = "");
    void error(const std::string &message, const std::string function_prefix = "");

} // namespace logger
