#pragma once

#include <string>
namespace storage
{
    namespace server
    {
        class PagesManager
        {
        public:
            PagesManager() = delete;
            PagesManager(const std::string &html_folder_path);
            ~PagesManager() = default;

            std::string getHomePage();

        private:
            const std::string kHtmlFolderPath_;
        };
    }
}
