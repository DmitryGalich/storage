#include "gtest/gtest.h"

#include <chrono>
#include <thread>

#include "logger.h"

class LogTests : public ::testing::Test
{
protected:
  void SetUp() override
  {
    if (std::filesystem::is_directory(kTestsFolderPath_))
      std::filesystem::remove_all(kTestsFolderPath_);

    if (!std::filesystem::create_directory(kTestsFolderPath_))
      throw std::runtime_error("Can't create directory \"" +
                               std::string(kTestsFolderPath_) + "\"");
  };
  void TearDown() override
  {
    logger::set_path("");

    if (std::filesystem::is_directory(kTestsFolderPath_))
      std::filesystem::remove_all(kTestsFolderPath_);
  };

protected:
  const std::filesystem::path kTestsFolderPath_{
      std::filesystem::temp_directory_path().c_str() +
      std::string("/TestsFolder/")};
};

TEST_F(LogTests, Multithreading)
{
  logger::set_path(kTestsFolderPath_);

  std::vector<std::unique_ptr<std::thread>> threads(10);
  for (size_t i = 0; i < threads.size(); i++)
  {
    threads[i].reset(new std::thread([&]()
                                     {
      for (int i = 0; i < 10; i++) {
        logger::info(std::to_string(i), __func__);
        logger::warning(std::to_string(i), __func__);
        logger::error(std::to_string(i), __func__);

        std::this_thread::sleep_for(std::chrono::microseconds(100));
      } }));
  }

  for (auto &thread : threads)
    if (thread->joinable())
      thread->join();
}

TEST_F(LogTests, CheckFileRecreating)
{
  EXPECT_TRUE(std::filesystem::is_empty(kTestsFolderPath_));

  logger::set_path(kTestsFolderPath_);

  logger::info("Info message", __func__);
  logger::warning("Warning message", __func__);
  logger::error("Error message", __func__);

  EXPECT_FALSE(std::filesystem::is_empty(kTestsFolderPath_));

  int files_count = 0;
  for (const auto &entry :
       std::filesystem::directory_iterator(kTestsFolderPath_))
  {
    if (files_count > 1)
      FAIL();

    files_count++;

    std::size_t found = entry.path().string().find(".log");
    if (found == std::string::npos)
      FAIL();
  }
}

TEST_F(LogTests, WithoutSettingPath)
{
  EXPECT_TRUE(std::filesystem::is_empty(kTestsFolderPath_));

  logger::info("Info message", __func__);
  logger::warning("Warning message", __func__);
  logger::error("Error message", __func__);

  EXPECT_TRUE(std::filesystem::is_empty(kTestsFolderPath_));
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
