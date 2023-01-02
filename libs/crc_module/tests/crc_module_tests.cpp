#include <gtest/gtest.h>

#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <list>

#include "crc_module.h"

struct CrcTestsHandler : public testing::Test
{
    void SetUp()
    {
        if (std::filesystem::exists(kTmpFolderPath_))
            std::filesystem::remove_all(kTmpFolderPath_);

        std::filesystem::create_directories(kTmpFolderPath_);
    }

    void TearDown()
    {
        std::filesystem::remove_all(kTmpFolderPath_);
    }

    bool create_file(const std::filesystem::path &relative_path, const std::string &content)
    {
        const std::string kFilepath = static_cast<std::string>(kTmpFolderPath_) + static_cast<std::string>(relative_path);

        std::ofstream file(kFilepath);
        file << content;
        file.close();

        return std::filesystem::exists(kFilepath);
    }

    const std::filesystem::path kTmpFolderPath_{std::filesystem::temp_directory_path().c_str() + static_cast<std::string>("/tests_folder/")};
};

TEST_F(CrcTestsHandler, SameCrc)
{
    {
        const std::string kData; // Empty

        const auto kCrc0 = crc_module::compute_crc(kData.c_str());
        const auto kCrc1 = crc_module::compute_crc(kData.c_str());

        EXPECT_EQ(kCrc0, kCrc1);
    }

    {
        const std::string kData{"Hello world"};

        const auto kCrc0 = crc_module::compute_crc(kData.c_str());
        const auto kCrc1 = crc_module::compute_crc(kData.c_str());

        EXPECT_EQ(kCrc0, kCrc1);
    }
}

TEST_F(CrcTestsHandler, FileCrc)
{
    const std::filesystem::path kFilepath("file.txt");

    EXPECT_TRUE(CrcTestsHandler::create_file(kFilepath, "Hello world"));

    std::ifstream file(kTmpFolderPath_.c_str() + static_cast<std::string>(kFilepath));
    EXPECT_TRUE(file.is_open());

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    crc_module::compute_crc(buffer.str().c_str());
}