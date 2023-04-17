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
        PagesManager::PagesManager(const std::string &html_folder_path)
            : kHtmlFolderPath_(html_folder_path) {}

        std::string PagesManager::getHomePage()
        {
            static const std::string kFilepath(kHtmlFolderPath_ + "/home/index.html");
            LOG(INFO) << "Loading file: \"" << kFilepath << "\"";
            return load_file(kFilepath);
        }

        std::string PagesManager::getPageNotFoundPage()
        {
            static const std::string kFilepath(kHtmlFolderPath_ + "statuses/404/index.html");
            LOG(INFO) << "Loading file: \"" << kFilepath << "\"";
            return load_file(kFilepath);
        }

        std::string PagesManager::getKekPage()
        {
            static const std::string kFilepath(kHtmlFolderPath_ + "kek/index.html");
            LOG(INFO) << "Loading file: \"" << kFilepath << "\"";
            return load_file(kFilepath);
        }
    }
}