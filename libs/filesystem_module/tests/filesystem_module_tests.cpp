#include <gtest/gtest.h>

#include "filesystem_module.h"

#include <string>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

struct FilesystemTestsHandler : public testing::Test
{
    void SetUp()
    {
        if (fs::exists(kTmpFolderPath_))
            fs::remove_all(kTmpFolderPath_);

        fs::create_directories(kTmpFolderPath_);
    }

    void TearDown()
    {
        fs::remove_all(kTmpFolderPath_);
    }

    const fs::path kTmpFolderPath_{fs::temp_directory_path().c_str() + std::string("/tests_folder/")};
};

TEST_F(FilesystemTestsHandler, GetListOfContent_EmptyFolder)
{
    auto list = filesystem_module::FilesystemModule::get_list_of_content(kTmpFolderPath_);

    EXPECT_TRUE(list.empty());
}

TEST_F(FilesystemTestsHandler, GetListOfContent_FolderWithFiles)
{
    const std::list<fs::path> kRealPaths{
        {kTmpFolderPath_.c_str() + std::string("first_file.txt")},
        {kTmpFolderPath_.c_str() + std::string("deep_folder/")},
        {kTmpFolderPath_.c_str() + std::string("deep_folder/second_file.txt")},
        {kTmpFolderPath_.c_str() + std::string("deep_folder/deepest_folder/")},
        {kTmpFolderPath_.c_str() + std::string("deep_folder/deepest_folder/third_file.txt")}};

    for (const auto &path : kRealPaths)
    {
        std::ofstream file(path);
        file.close();

        if (fs::exists(path))
            continue;

        EXPECT_TRUE(fs::create_directories(path.parent_path()));

        std::ofstream second_attempt_file(path);
        second_attempt_file.close();

        EXPECT_TRUE(fs::exists(path));
    }

    const auto kResultPaths = filesystem_module::FilesystemModule::get_list_of_content(kTmpFolderPath_);

    EXPECT_EQ(kResultPaths.size(), kRealPaths.size());

    for (const auto &result_path : kResultPaths)
    {
        bool is_found = false;

        for (auto real_path : kRealPaths)
        {
            if (real_path.string().back() == '/')
                real_path = real_path.parent_path();

            if (result_path == real_path)
            {
                is_found = true;
                break;
            }
        }

        EXPECT_TRUE(is_found);
    }
}