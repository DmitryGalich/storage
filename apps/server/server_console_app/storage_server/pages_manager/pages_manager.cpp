#include "pages_manager.hpp"

#include "easylogging++.h"

#include <fstream>
namespace
{
    std::string load_file(const std::string file_path)
    {
        std::fstream file_stream(file_path);
        if (!file_stream)
        {
            LOG(ERROR) << "Can't load file: \"" << file_path << "\"";
            return {};
        }

        std::ostringstream ss;
        ss << f.rdbuf();
        return ss.str();
    }
}

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