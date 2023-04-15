#include "pages_manager.hpp"

#include "easylogging++.h"

#include <fstream>
namespace
{
    std::string load_file(const std::string &file_path)
    {
        std::fstream file_stream(file_path);
        if (!file_stream)
        {
            LOG(ERROR) << "Can't load file: \"" << file_path << "\"";
            return {};
        }

        std::ostringstream string_stream;
        string_stream << file_stream.rdbuf();
        return string_stream.str();
    }
}

namespace storage
{
    namespace server
    {
        PagesManager::PagesManager() {}

        std::string PagesManager::getHomePage(const std::string &file_path)
        {
            LOG(INFO) << "Loading file: \"" << file_path << "\"";
            return load_file(file_path);
        }
    }
}