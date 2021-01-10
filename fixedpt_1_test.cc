#include "fixedpt.h"

#include <gtest/gtest.h>
using ::testing::InitGoogleTest;

using fixed::Num;
using std::string;

namespace {

long double number(const string& s) { return strtold(s.c_str(), NULL); }

TEST(FixedTest, String) {
  Num five(5);
  Num half(0.5);
  Num twenty(20);
  EXPECT_EQ(string(five), "5") << "Num of integer";
  EXPECT_EQ(string(half), "5e-1") << "Num of floting point";
  EXPECT_EQ(string(twenty), "20") << "Num of larger integer";
}

TEST(FixedTest, StringAsFloating) {
  Num five(5);
  Num half(0.5);
  EXPECT_EQ(number(string(five)), 5.0L) << "Num of integer";
  EXPECT_EQ(number(string(half)), 0.5L) << "Num of floting point";
}

TEST(FixedTest, Multiply) {
  Num five(5);
  Num four(4);
  Num twenty = five * four;
  EXPECT_EQ(string(twenty), "20");
}
}  // namespace

int main(int argc, char** argv) {
  InitGoogleTest(&argc, argv);
  Num::init(1);  // numbers scaled to 0.5
  return RUN_ALL_TESTS();
}