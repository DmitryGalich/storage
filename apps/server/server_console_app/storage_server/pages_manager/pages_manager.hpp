#pragma once

#include <string>
namespace storage
{
    namespace server
    {
        class PagesManager
        {
        public:
            PagesManager();
            ~PagesManager() = default;

            std::string getHomePage();
        };
    }
}
