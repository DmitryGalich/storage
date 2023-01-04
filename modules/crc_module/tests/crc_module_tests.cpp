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

    const std::filesystem::path kFilepath_{"file.txt"};
    const std::string kContent{"Hello world"};
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
        const auto kCrc0 = crc_module::compute_crc(kContent.c_str());
        const auto kCrc1 = crc_module::compute_crc(kContent.c_str());

        EXPECT_EQ(kCrc0, kCrc1);
    }
}

TEST_F(CrcTestsHandler, FullFileContentCrcViaStringstream)
{
    EXPECT_TRUE(CrcTestsHandler::create_file(kFilepath_, kContent));

    std::ifstream file(kTmpFolderPath_.c_str() + static_cast<std::string>(kFilepath_));
    EXPECT_TRUE(file.is_open());

    std::stringstream content_from_file;
    content_from_file << file.rdbuf();
    file.close();

    const auto kContentCrc = crc_module::compute_crc(kContent.c_str());
    const auto kContentFromFilerc = crc_module::compute_crc(content_from_file.str().c_str());
    EXPECT_EQ(kContentCrc, kContentFromFilerc);
}

TEST_F(CrcTestsHandler, FullFileContentCrcViaRead)
{
    EXPECT_TRUE(CrcTestsHandler::create_file(kFilepath_, kContent));

    std::ifstream file(kTmpFolderPath_.c_str() + static_cast<std::string>(kFilepath_));
    EXPECT_TRUE(file.is_open());

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    std::string content_from_file(size, ' ');
    file.seekg(0);
    file.read(&content_from_file[0], size);
    file.close();

    const auto kContentCrc = crc_module::compute_crc(kContent.c_str());
    const auto kContentFromFilerc = crc_module::compute_crc(content_from_file.c_str());
    EXPECT_EQ(kContentCrc, kContentFromFilerc);
}

TEST_F(CrcTestsHandler, FileChunkCrc)
{
    const std::string kFileChunk{"Hello"};

    EXPECT_TRUE(CrcTestsHandler::create_file(kFilepath_, kContent));

    std::ifstream file(kTmpFolderPath_.c_str() + static_cast<std::string>(kFilepath_));
    EXPECT_TRUE(file.is_open());

    std::string content_from_file(kFileChunk.size(), ' ');
    file.seekg(0);
    file.read(&content_from_file[0], kFileChunk.size());
    file.close();

    const auto kFileChunkCrc = crc_module::compute_crc(kFileChunk.c_str());
    const auto kContentFromFilerc = crc_module::compute_crc(content_from_file.c_str());
    EXPECT_EQ(kFileChunkCrc, kContentFromFilerc);
}