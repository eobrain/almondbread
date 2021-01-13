#include <gtest/gtest.h>

#include "fixedpt.h"
using ::testing::InitGoogleTest;

using fixed::Num;
using std::string;

namespace {

long double number(const string& s) { return strtold(s.c_str(), NULL); }

TEST(FixedTest, String) {
  Num smallest(1e-17);
  Num small(4e-15);
  Num bigger(0.01953125);
  Num half(0.5);
  Num integer(2.0);
  EXPECT_EQ(string(smallest), "1e-17") << "Smallest";
  EXPECT_EQ(string(small), "400e-17") << "Small";
  EXPECT_EQ(string(bigger), "1953125000000000e-17") << "Bigger";
  EXPECT_EQ(string(half), "50000000000000000e-17") << "Half";
  EXPECT_EQ(string(integer), "200000000000000000e-17") << "Integer";
}

TEST(FixedTest, StringAsFloating) {
  Num smallest(1e-17);
  Num small(4e-15);
  Num bigger(0.01953125);
  EXPECT_FLOAT_EQ(number(string(smallest)), 1e-17) << "Smallest";
  EXPECT_FLOAT_EQ(number(string(small)), 4e-15) << "Small";
  EXPECT_FLOAT_EQ(number(string(bigger)), 0.01953125) << "Bigger";
}

TEST(FixedTest, Multiply) {
  Num a(2e-7);
  Num b(3e-7);
  Num ab = a * b; // 6e-14
  EXPECT_EQ(string(ab), "6000e-17");
}

TEST(FixedTest, MultiplyNotUnderflow) {
  Num a(1.1111e-10);
  Num b(7e-6);
  Num ab = a * b;  // 7.7777e-16
  EXPECT_EQ(string(ab), "77e-17");
}

TEST(FixedTest, MultiplyInteger) {
  Num a(0.25);
  Num ab = a * 2;
  EXPECT_EQ(string(ab), "50000000000000000e-17");
}

TEST(FixedTest, Add) {
  Num a(11e-17);
  Num b(22e-17);
  Num ab = a + b;
  EXPECT_EQ(string(ab), "33e-17");
}

TEST(FixedTest, AddInteger) {
  Num a(0.25);
  Num ab = a + 2;
  EXPECT_EQ(string(ab), "225000000000000000e-17");
}

TEST(FixedTest, Subtract) {
  Num a(8e-17);
  Num b(3e-17);
  Num ab = a - b;
  Num ba = b - a;
  EXPECT_EQ(string(ab), "5e-17");
  EXPECT_EQ(string(ba), "-5e-17");
}

TEST(FixedTest, SubtractInteger) {
  Num a(3.5);
  Num ab = a - 2;
  EXPECT_EQ(string(ab), "150000000000000000e-17");
}

TEST(FixedTest, DivideInteger) {
  Num a(1000e-17);
  Num ab = a / 4;
  EXPECT_EQ(string(ab), "250e-17");
}

TEST(FixedTest, Parse) {
  Num smallest("1e-17");
  Num small("4e-15");
  Num bigger("0.01953125");
  Num half("0.5");
  Num integer("3.0");
  EXPECT_EQ(string(smallest), "1e-17") << "Smallest";
  EXPECT_EQ(string(small), "400e-17") << "Small";
  EXPECT_EQ(string(bigger), "1953125000000000e-17") << "Bigger";
  EXPECT_EQ(string(half), "50000000000000000e-17") << "Half";
  EXPECT_EQ(string(integer), "300000000000000000e-17") << "Integer";
}

}  // namespace

int main(int argc, char** argv) {
  InitGoogleTest(&argc, argv);
  Num::init(17);
  return RUN_ALL_TESTS();
}