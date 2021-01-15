#include <gtest/gtest.h>

#include "fixedpt.h"
using ::testing::InitGoogleTest;

using fixed::init;
using fixed::Num;
using fixed::toNum;
using std::string;

namespace {

long double number(const string& s) { return strtold(s.c_str(), NULL); }

TEST(FixedTest, String) {
  Num five = toNum(5);
  Num half = toNum(0.1);
  Num twenty = toNum(20);
  EXPECT_EQ(toString(five), "50e-1") << "Num of integer";
  EXPECT_EQ(toString(half), "1e-1") << "Num of floting point";
  EXPECT_EQ(toString(twenty), "200e-1") << "Num of larger integer";
}

TEST(FixedTest, StringAsFloating) {
  Num five = toNum(5);
  Num half = toNum(0.5);
  EXPECT_EQ(number(toString(five)), 5.0L) << "Num of integer";
  EXPECT_EQ(number(toString(half)), 0.5L) << "Num of floting point";
}

TEST(FixedTest, Multiply) {
  Num five = toNum(5);
  Num four = toNum(4);
  Num twenty = five * four;
  EXPECT_EQ(toString(twenty), "200e-1");
}
}  // namespace

int main(int argc, char** argv) {
  InitGoogleTest(&argc, argv);
  init(1);  // numbers scaled to 0.1
  return RUN_ALL_TESTS();
}