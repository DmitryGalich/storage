#include "network.h"

#include <chrono>
#include <memory>
#include <thread>

#include "gtest/gtest.h"

#include "logger.h"

class NetworkTests : public ::testing::Test {
protected:
  void SetUp() override {
    if (std::filesystem::is_directory(kTestsFolderPath_))
      std::filesystem::remove_all(kTestsFolderPath_);

    if (!std::filesystem::create_directory(kTestsFolderPath_))
      throw std::runtime_error("Can't create directory \"" +
                               std::string(kTestsFolderPath_) + "\"");

    log::set_path(kTestsFolderPath_);
  };
  void TearDown() override {
    log::clear_path();

    if (std::filesystem::is_directory(kTestsFolderPath_))
      std::filesystem::remove_all(kTestsFolderPath_);
  };

  const std::filesystem::path kTestsFolderPath_{
      std::filesystem::temp_directory_path().c_str() +
      std::string("/TestsFolder/")};
  network::Server server_;
};

// TEST_F(NetworkTests, IncorrectIp) { EXPECT_ANY_THROW(server_.start("",
// 8080)); }

TEST_F(NetworkTests, Simple) {
  using namespace std::chrono_literals;
  EXPECT_NO_THROW(server_.start("127.0.0.1", 8080));
  std::this_thread::sleep_for(5s);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
