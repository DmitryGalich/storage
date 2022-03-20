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

  //  network::Server server_;
  network::Client client_;
};

TEST_F(NetworkTests, Simple) {
  using namespace std::chrono_literals;

  const std::string kIp("127.0.0.1");
  const uint32_t kPort(8080);

  //  std::thread server_thread(
  //      [&]() { EXPECT_NO_THROW(server_.start(kIp, kPort)); });

  std::thread client_thread(
      [&]() { EXPECT_NO_THROW(client_.start(kIp, kPort)); });

  std::this_thread::sleep_for(5s);

  //  server_thread.join();
  client_thread.join();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
