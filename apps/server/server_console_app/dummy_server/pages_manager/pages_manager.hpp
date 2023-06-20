#pragma once

#include <string>
namespace dummy
{
    namespace server
    {
        class PagesManager
        {
        public:
            PagesManager() = delete;
            PagesManager(const std::string &html_folder_path);
            ~PagesManager() = default;

            std::string getPageNotFoundPage();
            std::string getHomePage();
            std::string getKekPage();

        private:
            const std::string kHtmlFolderPath_;
        };
    }
}
