#include "filesystem_module.h"

namespace filesystem_module
{
    std::list<std::filesystem::path> FilesystemModule::get_list_of_content(const std::filesystem::path &path)
    {
        std::list<std::filesystem::path> list;

        for (const auto &entry : std::filesystem::recursive_directory_iterator(path))
            list.emplace_back(entry);

        return list;
    }
}