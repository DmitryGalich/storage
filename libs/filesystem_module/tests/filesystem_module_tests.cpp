#include <gtest/gtest.h>

#include "filesystem_module.h"

TEST(FilesystemTests, SimpleTest)
{
    filesystem_module::FilesystemModule module;
    auto list = module.get_list_of_content("");
}