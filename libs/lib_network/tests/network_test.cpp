#include "network.h"

#include <memory>

#include "gtest/gtest.h"

class NetworkTests : public ::testing::Test {
protected:
  void SetUp() override{};
  void TearDown() override{};

  network::Server server_;
};

TEST_F(NetworkTests, IncorrectIp) { EXPECT_ANY_THROW(server_.start("", 8080)); }

TEST_F(NetworkTests, Simple) {
  EXPECT_NO_THROW(server_.start("127.0.0.1", 8080));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
