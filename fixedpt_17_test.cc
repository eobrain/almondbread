#include <gtest/gtest.h>

#include "fixedpt.h"
using ::testing::InitGoogleTest;

using fixed::init;
using fixed::Num;
using fixed::parse;
using fixed::toNum;
using std::string;

namespace {

long double number(const string& s) { return strtold(s.c_str(), NULL); }

TEST(FixedTest, String) {
  Num smallest = toNum(1e-17);
  Num small = toNum(4e-15);
  Num bigger = toNum(0.01953125);
  Num half = toNum(0.5);
  Num integer = toNum(2.0);
  EXPECT_EQ(toString(smallest), "1e-17") << "Smallest";
  EXPECT_EQ(toString(small), "400e-17") << "Small";
  EXPECT_EQ(toString(bigger), "1953125000000000e-17") << "Bigger";
  EXPECT_EQ(toString(half), "50000000000000000e-17") << "Half";
  EXPECT_EQ(toString(integer), "200000000000000000e-17") << "Integer";
}

TEST(FixedTest, StringAsFloating) {
  Num smallest = toNum(1e-17);
  Num small = toNum(4e-15);
  Num bigger = toNum(0.01953125);
  EXPECT_FLOAT_EQ(number(toString(smallest)), 1e-17) << "Smallest";
  EXPECT_FLOAT_EQ(number(toString(small)), 4e-15) << "Small";
  EXPECT_FLOAT_EQ(number(toString(bigger)), 0.01953125) << "Bigger";
}

TEST(FixedTest, Multiply) {
  Num a = toNum(2e-7);
  Num b = toNum(3e-7);
  Num ab = a * b;  // 6e-14
  EXPECT_EQ(toString(ab), "6000e-17");
}

TEST(FixedTest, MultiplyNotUnderflow) {
  Num a = toNum(1.1111e-10);
  Num b = toNum(7e-6);
  Num ab = a * b;  // 7.7777e-16
  EXPECT_EQ(toString(ab), "77e-17");
}

TEST(FixedTest, MultiplyInteger) {
  Num a = toNum(0.25);
  Num ab = a * 2;
  EXPECT_EQ(toString(ab), "50000000000000000e-17");
}

TEST(FixedTest, Add) {
  Num a = toNum(11e-17);
  Num b = toNum(22e-17);
  Num ab = a + b;
  EXPECT_EQ(toString(ab), "33e-17");
}

TEST(FixedTest, AddInteger) {
  Num a = toNum(0.25);
  Num ab = a + 2;
  EXPECT_EQ(toString(ab), "225000000000000000e-17");
}

TEST(FixedTest, Subtract) {
  Num a = toNum(8e-17);
  Num b = toNum(3e-17);
  Num ab = a - b;
  Num ba = b - a;
  EXPECT_EQ(toString(ab), "5e-17");
  EXPECT_EQ(toString(ba), "-5e-17");
}

TEST(FixedTest, SubtractInteger) {
  Num a = toNum(3.5);
  Num ab = a - 2;
  EXPECT_EQ(toString(ab), "150000000000000000e-17");
}

TEST(FixedTest, DivideInteger) {
  Num a = toNum(1000e-17);
  Num ab = a / 4;
  EXPECT_EQ(toString(ab), "250e-17");
}

TEST(FixedTest, Parse) {
  Num smallest = parse("1e-17");
  Num small = parse("4e-15");
  Num bigger = parse("0.01953125");
  Num half = parse("0.5");
  Num integer = parse("3.0");
  EXPECT_EQ(toString(smallest), "1e-17") << "Smallest";
  EXPECT_EQ(toString(small), "400e-17") << "Small";
  EXPECT_EQ(toString(bigger), "1953125000000000e-17") << "Bigger";
  EXPECT_EQ(toString(half), "50000000000000000e-17") << "Half";
  EXPECT_EQ(toString(integer), "300000000000000000e-17") << "Integer";
}

}  // namespace

int main(int argc, char** argv) {
  InitGoogleTest(&argc, argv);
  init(17);
  return RUN_ALL_TESTS();
}