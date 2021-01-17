#include <gtest/gtest.h>

#include <regex>

#include "fixedpt.h"
using ::testing::InitGoogleTest;

using fixed::init;
using fixed::Num;
using fixed::parse;
using fixed::toNum;
using std::regex;
using std::regex_match;
using std::string;

namespace {

long double number(const string& s) { return strtold(s.c_str(), NULL); }

TEST(FixedTest, String) {
  Num smallest = toNum(1e-35);
  Num small = toNum(4e-33);
  Num bigger = toNum(0.01953125L);
  Num half = toNum(0.5);
  Num integer = toNum(2.0);
  EXPECT_EQ(toString(smallest), "1e-35") << "Smallest";
  EXPECT_EQ(toString(small), "400e-35") << "Small";
  EXPECT_TRUE(regex_match(toString(bigger),
                          regex("19531249999999999999..............e-35")))
      << toString(bigger);
  EXPECT_TRUE(regex_match(toString(half),
                          regex("4999999999999999...................e-35")))
      << toString(half);
  EXPECT_TRUE(regex_match(toString(integer),
                          regex("19999999999999999...................e-35")))

      << toString(integer);
}

TEST(FixedTest, StringAsFloating) {
  Num smallest = toNum(1e-35);
  Num small = toNum(4e-15);
  Num bigger = toNum(0.01953125);
  EXPECT_FLOAT_EQ(number(toString(smallest)), 1e-35) << "Smallest";
  EXPECT_FLOAT_EQ(number(toString(small)), 4e-15) << "Small";
  EXPECT_FLOAT_EQ(number(toString(bigger)), 0.01953125) << "Bigger";
}

TEST(FixedTest, Multiply) {
  Num a = toNum(2e-15);
  Num b = toNum(3e-15);
  Num ab = a * b;  // 6e-30
  EXPECT_EQ(toString(ab), "600000e-35");
}

TEST(FixedTest, MultiplyNotUnderflow) {
  Num a = toNum(1.1111e-10);
  Num b = toNum(7e-6);
  Num ab = a * b;  // 7.7777e-16
  EXPECT_FLOAT_EQ(number(toString(ab)), 7.7777e-16);
}

TEST(FixedTest, MultiplyUnderflow) {
  Num a = toNum(1.1111e-10);
  Num b = toNum(7e-24);
  Num ab = a * b;  // 7.7777e-34
  EXPECT_EQ(toString(ab), "77e-35");
}

TEST(FixedTest, MultiplyInteger) {
  Num a = toNum(0.25);
  Num ab = a * 2;
  EXPECT_FLOAT_EQ(number(toString(ab)), 0.5);
}

TEST(FixedTest, Add) {
  Num a = parse("11e-35");
  Num b = parse("22e-35");
  Num ab = a + b;
  EXPECT_EQ(toString(a), "11e-35");
  EXPECT_EQ(toString(b), "22e-35");
  EXPECT_EQ(toString(ab), "33e-35");
}

TEST(FixedTest, AddInteger) {
  Num a = toNum(0.25);
  Num ab = a + 2;
  EXPECT_FLOAT_EQ(number(toString(ab)), 2.25);
}

TEST(FixedTest, Subtract) {
  Num a = toNum(8e-35);
  Num b = toNum(3e-35);
  Num ab = a - b;
  Num ba = b - a;
  EXPECT_EQ(toString(ab), "5e-35");
  EXPECT_EQ(toString(ba), "-5e-35");
}

TEST(FixedTest, SubtractInteger) {
  Num a = toNum(3.5);
  Num ab = a - 2;
  EXPECT_FLOAT_EQ(number(toString(ab)), 1.5);
}

TEST(FixedTest, DivideInteger) {
  Num a = toNum(1000e-35);
  Num ab = a / 4;
  EXPECT_EQ(toString(ab), "250e-35");
}

TEST(FixedTest, DoubleToNum) {
  Num smallest = toNum(1e-35);
  Num small = toNum(4e-30);
  Num bigger = toNum(0.01953125);
  Num half = toNum(0.5);
  Num integer = toNum(3.0);
  EXPECT_EQ(toString(smallest), "1e-35") << "Smallest";
  EXPECT_EQ(toString(small), "400000e-35") << "Small";
  EXPECT_FLOAT_EQ(number(toString(bigger)), 0.01953125) << "Bigger";
  EXPECT_FLOAT_EQ(number(toString(half)), 0.5) << "Half";
  EXPECT_FLOAT_EQ(number(toString(integer)), 3.0) << "Integer";
}

TEST(FixedTest, LongDoubleToNum) {
  Num smallest = toNum(1e-35L);
  Num small = toNum(4e-30L);
  Num bigger = toNum(0.01953125L);
  Num half = toNum(0.5L);
  Num integer = toNum(3.0L);
  EXPECT_EQ(toString(smallest), "1e-35") << "Smallest";
  EXPECT_EQ(toString(small), "400000e-35") << "Small";
  EXPECT_FLOAT_EQ(number(toString(bigger)), 0.01953125L) << "Bigger";
  EXPECT_FLOAT_EQ(number(toString(half)), 0.5L) << "Half";
  EXPECT_FLOAT_EQ(number(toString(integer)), 3.0L) << "Integer";
}

TEST(FixedTest, Parse) {
  Num smallest = parse("1e-35");
  Num small = parse("4e-30");
  Num bigger = parse("0.01953125");
  Num half = parse("0.5");
  Num integer = parse("3.0");
  EXPECT_EQ(toString(smallest), "1e-35") << "Smallest";
  EXPECT_EQ(toString(small), "400000e-35") << "Small";
  EXPECT_EQ(toString(bigger), "1953125000000000000000000000000000e-35")
      << "Bigger";
  EXPECT_EQ(toString(half), "50000000000000000000000000000000000e-35")
      << "Half";
  EXPECT_EQ(toString(integer), "300000000000000000000000000000000000e-35")
      << "Integer";
}

}  // namespace

int main(int argc, char** argv) {
  InitGoogleTest(&argc, argv);
  init(35);
  return RUN_ALL_TESTS();
}