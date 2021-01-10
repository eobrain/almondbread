#include <gtest/gtest.h>

#include "fixedpt.h"
using ::testing::InitGoogleTest;

using fixed::Num;
using std::string;

namespace {

long double number(const string& s) { return strtold(s.c_str(), NULL); }

TEST(FixedTest, String) {
  Num smallest(3.90625e-3L);  // 1/256
  Num bigger(0.01953125L);    // 5/256
  Num half(0.5L);             // 128/256
  Num integer(3.0L);
  EXPECT_EQ(string(smallest), "3.90625e-3") << "Smallest";
  EXPECT_EQ(string(bigger), "1.953125e-2") << "Bigger";
  EXPECT_EQ(string(half), "5e-1") << "Half";
  EXPECT_EQ(string(integer), "3") << "Integer";
}

TEST(FixedTest, StringAsFloating) {
  Num smallest(3.90625e-3L);  // 1/256
  Num bigger(0.01953125L);    // 5/256
  EXPECT_EQ(number(string(smallest)), 3.90625e-3L) << "Smallest";
  EXPECT_EQ(number(string(bigger)), 0.01953125L) << "Bigger";
}

TEST(FixedTest, Multiply) {
  Num a(0.25L);    // 64/256
  Num b(0.125L);   // 32/256
  Num ab = a * b;  // 1/32 = 0.03125
  EXPECT_EQ(string(ab), "3.125e-2");
}

TEST(FixedTest, MultiplyUnderflow) {
  Num a(1.953125e-2L);  // 5/256
  Num b(7.8125e-3L);    // 2/256
  Num ab = a * b;       // 0.00015258789 if floating point
  EXPECT_EQ(string(ab), "0");
}

TEST(FixedTest, MultiplyInteger) {
  Num a(0.25L);
  Num ab = a * 2LL;
  EXPECT_EQ(string(ab), "5e-1");
}

TEST(FixedTest, Add) {
  Num a(0.25L);   // 64/256
  Num b(0.125L);  // 32/256
  Num ab = a + b;
  EXPECT_EQ(string(ab), "3.75e-1");
}

TEST(FixedTest, AddInteger) {
  Num a(0.25L);
  Num ab = a + 2LL;
  EXPECT_EQ(string(ab), "2.25");
}

TEST(FixedTest, Subtract) {
  Num a(0.75L);
  Num b(0.125L);
  Num ab = a - b;
  EXPECT_EQ(string(ab), "6.25e-1");
}

TEST(FixedTest, SubtractInteger) {
  Num a(3.5L);
  Num ab = a - 2LL;
  EXPECT_EQ(string(ab), "1.5");
}

TEST(FixedTest, DivideInteger) {
  Num a(0.5L);
  Num ab = a / 4LL;
  EXPECT_EQ(string(ab), "1.25e-1");
}

/*TEST(FixedTest, Parse) {
  Num smallest("3.90625e-3");  // 1/256
  Num bigger("0.01953125");    // 5/256
  Num half("0.5");             // 128/256
  Num integer("3.0");
  EXPECT_EQ(string(smallest), "3.90625e-3") << "Smallest";
  EXPECT_EQ(string(bigger), "1.953125e-2") << "Bigger";
  EXPECT_EQ(string(half), "5e-1") << "Half";
  EXPECT_EQ(string(integer), "3") << "Integer";
}*/

}  // namespace

int main(int argc, char** argv) {
  InitGoogleTest(&argc, argv);
  Num::init(8);  // numbers scaled to 1/256 = 0.00390625 = 3.90625e-3
  return RUN_ALL_TESTS();
}