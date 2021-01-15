#include <gtest/gtest.h>

#include <regex>

#include "fixedpt.h"
using fixed::init;
using fixed::Num;
using fixed::parse;
using fixed::toNum;
using std::regex;
using std::regex_match;
using std::string;
using ::testing::InitGoogleTest;

namespace {

long double number(const string& s) { return strtold(s.c_str(), NULL); }

TEST(FixedTest, String) {
  Num smallest = toNum(1e-12);
  Num small = toNum(4e-7);
  Num bigger = toNum(0.01953125);
  Num half = toNum(0.5);
  Num integer = toNum(2.0);
  EXPECT_EQ(toString(smallest), "1e-12") << "Smallest";
  EXPECT_EQ(toString(small), "400000e-12") << "Small";
  EXPECT_EQ(toString(bigger), "19531250000e-12") << "Bigger";
  EXPECT_EQ(toString(half), "500000000000e-12") << "Half";
  EXPECT_EQ(toString(integer), "2000000000000e-12") << "Integer";
}

TEST(FixedTest, StringAsFloating) {
  Num smallest = toNum(1e-12);
  Num small = toNum(4e-7);
  Num bigger = toNum(0.01953125);
  EXPECT_FLOAT_EQ(number(toString(smallest)), 1e-12) << "Smallest";
  EXPECT_FLOAT_EQ(number(toString(small)), 4e-7) << "Small";
  EXPECT_FLOAT_EQ(number(toString(bigger)), 0.01953125) << "Bigger";
}

TEST(FixedTest, Multiply) {
  Num a = toNum(3e-5);
  Num b = toNum(2e-5);
  Num ab = a * b;  // 6e-10
  EXPECT_EQ(toString(ab), "600e-12");
}

TEST(FixedTest, MultiplyNotUnderflow) {
  Num a = toNum(1.953125e-5);
  Num b = toNum(7.8125e-5);
  Num ab = a * b;
  EXPECT_EQ(toString(ab), "1525e-12");
}

TEST(FixedTest, MultiplyInteger) {
  Num a = toNum(0.25);
  Num ab = a * 2;
  EXPECT_EQ(toString(ab), "500000000000e-12");
}

TEST(FixedTest, Add) {
  Num a = toNum(11e-12);
  Num b = toNum(22e-12);
  Num ab = a + b;
  EXPECT_EQ(toString(ab), "33e-12");
}

TEST(FixedTest, AddInteger) {
  Num a = toNum(0.25);
  Num ab = a + 2;
  EXPECT_EQ(toString(ab), "2250000000000e-12");
}

TEST(FixedTest, Subtract) {
  Num a = toNum(8e-12);
  Num b = toNum(3e-12);
  Num ab = a - b;
  Num ba = b - a;
  EXPECT_EQ(toString(ab), "5e-12");
  EXPECT_EQ(toString(ba), "-5e-12");
}

TEST(FixedTest, SubtractInteger) {
  Num a = toNum(3.5);
  Num ab = a - 2;
  EXPECT_EQ(toString(ab), "1500000000000e-12");
}

TEST(FixedTest, DivideInteger) {
  Num a = toNum(1000e-12);
  Num ab = a / 4;
  EXPECT_EQ(toString(ab), "250e-12");
}

TEST(FixedTest, Parse) {
  Num smallest = parse("1e-12");
  Num small = parse("4e-7");
  Num bigger = parse("0.01953125");
  Num half = parse("0.5");
  Num integer = parse("3.0");
  EXPECT_EQ(toString(smallest), "1e-12") << "Smallest";
  EXPECT_EQ(toString(small), "400000e-12") << "Small";
  EXPECT_EQ(toString(bigger), "19531250000e-12") << "Bigger";
  EXPECT_EQ(toString(half), "500000000000e-12") << "Half";
  EXPECT_EQ(toString(integer), "3000000000000e-12") << "Integer";
}

}  // namespace

int main(int argc, char** argv) {
  InitGoogleTest(&argc, argv);
  init(12);
  return RUN_ALL_TESTS();
}