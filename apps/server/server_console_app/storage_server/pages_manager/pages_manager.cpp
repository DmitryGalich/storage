#include "pages_manager.hpp"

#include "easylogging++.h"

namespace storage
{
    namespace server
    {
        PagesManager::PagesManager() {}

        std::string PagesManager::getHomePage()
        {
            LOG(INFO) << "KEK";
            return "<html>\n<head><title> Home</title></head>\n<body>\n<h1> Home</h1>\n</body>\n</html>\n";
        }
    }
}