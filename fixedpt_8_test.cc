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
  Num smallest = toNum(3.90625e-3);  // 1/256
  Num bigger = toNum(0.01953125);    // 5/256
  Num half = toNum(0.5);             // 128/256
  Num integer = toNum(3.0);
  EXPECT_EQ(toString(smallest), "390625e-8") << "Smallest";
  EXPECT_EQ(toString(bigger), "1953125e-8") << "Bigger";
  EXPECT_EQ(toString(half), "50000000e-8") << "Half";
  EXPECT_EQ(toString(integer), "300000000e-8") << "Integer";
}

TEST(FixedTest, StringAsFloating) {
  Num smallest = toNum(3.90625e-3);  // 1/256
  Num bigger = toNum(0.01953125);    // 5/256
  EXPECT_EQ(number(toString(smallest)), 3.90625e-3L) << "Smallest";
  EXPECT_EQ(number(toString(bigger)), 0.01953125L) << "Bigger";
}

TEST(FixedTest, Multiply1) {
  Num a = toNum(0.25);   // 64/256
  Num b = toNum(0.125);  // 32/256
  Num ab = a * b;        // 1/32 = 0.03125
  EXPECT_EQ(toString(ab), "3125000e-8");
}

TEST(FixedTest, Multiply2) {
  Num a = toNum(0.02);
  Num b = toNum(0.03);
  Num ab = a * b;
  EXPECT_EQ(toString(ab), "60000e-8");
}

TEST(FixedTest, MultiplyUnderflow) {
  Num a = toNum(5e-8);  // 5/256
  Num b = toNum(2e-8);  // 2/256
  Num ab = a * b;
  EXPECT_EQ(toString(ab), "0e-8");
}

TEST(FixedTest, MultiplyInteger) {
  Num a = toNum(0.25);
  Num ab = a * 2;
  EXPECT_EQ(toString(ab), "50000000e-8");
}

TEST(FixedTest, Add) {
  Num a = toNum(0.25);   // 64/256
  Num b = toNum(0.125);  // 32/256
  Num ab = a + b;
  EXPECT_EQ(toString(ab), "37500000e-8");
}

TEST(FixedTest, AddInteger) {
  Num a = toNum(0.25);
  Num ab = a + 2;
  EXPECT_EQ(toString(ab), "225000000e-8");
}

TEST(FixedTest, Subtract) {
  Num a = toNum(0.75);
  Num b = toNum(0.125);
  Num ab = a - b;
  EXPECT_EQ(toString(ab), "62500000e-8");
}

TEST(FixedTest, SubtractInteger) {
  Num a = toNum(3.5);
  Num ab = a - 2;
  EXPECT_EQ(toString(ab), "150000000e-8");
}

TEST(FixedTest, DivideInteger) {
  Num a = toNum(0.5);
  Num ab = a / 4;
  EXPECT_EQ(toString(ab), "12500000e-8");
}

TEST(FixedTest, Parse) {
  Num smallest = parse("3.90625e-3");
  Num bigger = parse("0.01953125");
  Num half = parse("0.5");
  Num integer = parse("3.0");
  EXPECT_EQ(toString(smallest), "390625e-8") << "Smallest";
  EXPECT_EQ(toString(bigger), "1953125e-8") << "Bigger";
  EXPECT_EQ(toString(half), "50000000e-8") << "Half";
  EXPECT_EQ(toString(integer), "300000000e-8") << "Integer";
}

}  // namespace

int main(int argc, char** argv) {
  InitGoogleTest(&argc, argv);
  init(8);  // numbers scaled to 1/256 = 0.00390625 = 3.90625e-3
  return RUN_ALL_TESTS();
}