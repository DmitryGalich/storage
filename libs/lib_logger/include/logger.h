#pragma once

#include <filesystem>
#include <string>
#include <thread>

namespace log {

void set_path(const std::filesystem::path& path);
void info(const std::string& message, const std::string usage_prefix = "");
void warning(const std::string& message, const std::string usage_prefix = "");
void error(const std::string& message, const std::string usage_prefix = "");

}  // namespace log
