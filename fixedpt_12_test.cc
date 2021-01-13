#include <gtest/gtest.h>

#include <regex>

#include "fixedpt.h"
using fixed::Num;
using std::regex;
using std::regex_match;
using std::string;
using ::testing::InitGoogleTest;

namespace {

long double number(const string& s) { return strtold(s.c_str(), NULL); }

TEST(FixedTest, String) {
  Num smallest(1e-12);
  Num small(4e-7);
  Num bigger(0.01953125);
  Num half(0.5);
  Num integer(2.0);
  EXPECT_EQ(string(smallest), "1e-12") << "Smallest";
  EXPECT_EQ(string(small), "400000e-12") << "Small";
  EXPECT_EQ(string(bigger), "19531250000e-12") << "Bigger";
  EXPECT_EQ(string(half), "500000000000e-12") << "Half";
  EXPECT_EQ(string(integer), "2000000000000e-12") << "Integer";
}

TEST(FixedTest, StringAsFloating) {
  Num smallest(1e-12);
  Num small(4e-7);
  Num bigger(0.01953125);
  EXPECT_FLOAT_EQ(number(string(smallest)), 1e-12) << "Smallest";
  EXPECT_FLOAT_EQ(number(string(small)), 4e-7) << "Small";
  EXPECT_FLOAT_EQ(number(string(bigger)), 0.01953125) << "Bigger";
}

TEST(FixedTest, Multiply) {
  Num a(3e-5);
  Num b(2e-5);
  Num ab = a * b;  // 6e-10
  EXPECT_EQ(string(ab), "600e-12");
}

TEST(FixedTest, MultiplyNotUnderflow) {
  Num a(1.953125e-5);
  Num b(7.8125e-5);
  Num ab = a * b;
  EXPECT_EQ(string(ab), "1525e-12");
}

TEST(FixedTest, MultiplyInteger) {
  Num a(0.25);
  Num ab = a * 2;
  EXPECT_EQ(string(ab), "500000000000e-12");
}

TEST(FixedTest, Add) {
  Num a(11e-12);
  Num b(22e-12);
  Num ab = a + b;
  EXPECT_EQ(string(ab), "33e-12");
}

TEST(FixedTest, AddInteger) {
  Num a(0.25);
  Num ab = a + 2;
  EXPECT_EQ(string(ab), "2250000000000e-12");
}

TEST(FixedTest, Subtract) {
  Num a(8e-12);
  Num b(3e-12);
  Num ab = a - b;
  Num ba = b - a;
  EXPECT_EQ(string(ab), "5e-12");
  EXPECT_EQ(string(ba), "-5e-12");
}

TEST(FixedTest, SubtractInteger) {
  Num a(3.5);
  Num ab = a - 2;
  EXPECT_EQ(string(ab), "1500000000000e-12");
}

TEST(FixedTest, DivideInteger) {
  Num a(1000e-12);
  Num ab = a / 4;
  EXPECT_EQ(string(ab), "250e-12");
}

TEST(FixedTest, Parse) {
  Num smallest("1e-12");
  Num small("4e-7");
  Num bigger("0.01953125");
  Num half("0.5");
  Num integer("3.0");
  EXPECT_EQ(string(smallest), "1e-12") << "Smallest";
  EXPECT_EQ(string(small), "400000e-12") << "Small";
  EXPECT_EQ(string(bigger), "19531250000e-12") << "Bigger";
  EXPECT_EQ(string(half), "500000000000e-12") << "Half";
  EXPECT_EQ(string(integer), "3000000000000e-12") << "Integer";
}

}  // namespace

int main(int argc, char** argv) {
  InitGoogleTest(&argc, argv);
  Num::init(12);
  return RUN_ALL_TESTS();
}