#define FILESYSTEM_MODULE
#ifdef FILESYSTEM_MODULE

#include <vector>
#include <string>

namespace filesystem_module
{

    class FilesystemModule
    {
    public:
        FilesystemModule() = default;
        ~FilesystemModule() = default;

        std::vector<std::string> get_list_of_content(const std::string &path);
    };
}

#endif