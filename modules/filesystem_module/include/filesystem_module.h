#define FILESYSTEM_MODULE
#ifdef FILESYSTEM_MODULE

#include <list>
#include <filesystem>

namespace filesystem_module
{

    class FilesystemModule
    {
    public:
        FilesystemModule() = delete;
        FilesystemModule(const FilesystemModule &) = delete;
        FilesystemModule(FilesystemModule &&) = delete;
        FilesystemModule &operator=(FilesystemModule &&) = delete;
        FilesystemModule &operator=(const FilesystemModule &) = delete;
        ~FilesystemModule() = default;

        static std::list<std::filesystem::path> get_list_of_content(const std::filesystem::path &path);
    };
}

#endif