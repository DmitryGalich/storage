#include <gtest/gtest.h>

#include "network_module.h"

#include <string>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

struct NetworkTestsHandler : public testing::Test
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

TEST_F(NetworkTestsHandler, ServerTest)
{
}
