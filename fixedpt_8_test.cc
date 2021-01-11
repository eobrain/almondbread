#include <gtest/gtest.h>

#include "fixedpt.h"
using ::testing::InitGoogleTest;

using fixed::Num;
using std::string;

namespace {

long double number(const string& s) { return strtold(s.c_str(), NULL); }

TEST(FixedTest, String) {
  Num smallest(3.90625e-3);  // 1/256
  Num bigger(0.01953125);    // 5/256
  Num half(0.5);             // 128/256
  Num integer(3.0);
  EXPECT_EQ(string(smallest), "3.90625e-3") << "Smallest";
  EXPECT_EQ(string(bigger), "1.953125e-2") << "Bigger";
  EXPECT_EQ(string(half), "5e-1") << "Half";
  EXPECT_EQ(string(integer), "3") << "Integer";
}

TEST(FixedTest, StringAsFloating) {
  Num smallest(3.90625e-3);  // 1/256
  Num bigger(0.01953125);    // 5/256
  EXPECT_EQ(number(string(smallest)), 3.90625e-3L) << "Smallest";
  EXPECT_EQ(number(string(bigger)), 0.01953125L) << "Bigger";
}

TEST(FixedTest, Multiply1) {
  Num a(0.25);     // 64/256
  Num b(0.125);    // 32/256
  Num ab = a * b;  // 1/32 = 0.03125
  EXPECT_EQ(string(ab), "3.125e-2");
}

TEST(FixedTest, Multiply2) {
  Num a(0.02);  
  Num b(0.03);  
  Num ab = a * b;  
  EXPECT_EQ(string(ab), "6e-4");
}

TEST(FixedTest, MultiplyUnderflow) {
  Num a(5e-8);  // 5/256
  Num b(2e-8);  // 2/256
  Num ab = a * b;
  EXPECT_EQ(string(ab), "0");
}

TEST(FixedTest, MultiplyInteger) {
  Num a(0.25);
  Num ab = a * 2;
  EXPECT_EQ(string(ab), "5e-1");
}

TEST(FixedTest, Add) {
  Num a(0.25);   // 64/256
  Num b(0.125);  // 32/256
  Num ab = a + b;
  EXPECT_EQ(string(ab), "3.75e-1");
}

TEST(FixedTest, AddInteger) {
  Num a(0.25);
  Num ab = a + 2;
  EXPECT_EQ(string(ab), "2.25");
}

TEST(FixedTest, Subtract) {
  Num a(0.75);
  Num b(0.125);
  Num ab = a - b;
  EXPECT_EQ(string(ab), "6.25e-1");
}

TEST(FixedTest, SubtractInteger) {
  Num a(3.5);
  Num ab = a - 2;
  EXPECT_EQ(string(ab), "1.5");
}

TEST(FixedTest, DivideInteger) {
  Num a(0.5);
  Num ab = a / 4;
  EXPECT_EQ(string(ab), "1.25e-1");
}

TEST(FixedTest, Parse) {
  Num smallest("3.90625e-3");
  Num bigger("0.01953125");
  Num half("0.5");
  Num integer("3.0");
  EXPECT_EQ(string(smallest), "3.90625e-3") << "Smallest";
  EXPECT_EQ(string(bigger), "1.953125e-2") << "Bigger";
  EXPECT_EQ(string(half), "5e-1") << "Half";
  EXPECT_EQ(string(integer), "3") << "Integer";
}

}  // namespace

int main(int argc, char** argv) {
  InitGoogleTest(&argc, argv);
  Num::init(8);  // numbers scaled to 1/256 = 0.00390625 = 3.90625e-3
  return RUN_ALL_TESTS();
}