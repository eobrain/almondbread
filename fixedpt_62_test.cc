#include <gtest/gtest.h>

#include "fixedpt.h"
using ::testing::InitGoogleTest;

using fixed::Num;
using std::string;

namespace {

long double number(const string& s) { return strtold(s.c_str(), NULL); }

TEST(FixedTest, String) {
  Num smallest(2.1684043e-19L);
  Num small(3.90625e-3L);
  Num bigger(0.01953125L);
  Num half(0.5L);
  Num integer(2.0L);
  EXPECT_EQ(string(smallest), "2.1684043e-19") << "Smallest";
  EXPECT_EQ(string(small), "3.90625e-3") << "Small";
  EXPECT_EQ(string(bigger), "1.953125e-2") << "Bigger";
  EXPECT_EQ(string(half), "5e-1") << "Half";
  EXPECT_EQ(string(integer), "2") << "Integer";
}

TEST(FixedTest, StringAsFloating) {
  Num smallest(2.1684043e-19L);
  Num small(3.90625e-3L);
  Num bigger(0.01953125L);
  EXPECT_EQ(number(string(smallest)), 2.1684043e-19L) << "Smallest";
  EXPECT_EQ(number(string(small)), 3.90625e-3L) << "Small";
  EXPECT_EQ(number(string(bigger)), 0.01953125L) << "Bigger";
}

TEST(FixedTest, Multiply) {
  Num a(0.25L);
  Num b(0.125L);
  Num ab = a * b;
  EXPECT_EQ(string(ab), "3.125e-2");
}

TEST(FixedTest, MultiplyNotUnderflow) {
  Num a(1.953125e-2L);
  Num b(7.8125e-3L);
  Num ab = a * b;
  EXPECT_EQ(string(ab), "1.5258789e-4");
}

TEST(FixedTest, MultiplyInteger) {
  Num a(0.25L);
  Num ab = a * 2LL;
  EXPECT_EQ(string(ab), "5e-1");
}

TEST(FixedTest, Add) {
  Num a(0.25L); 
  Num b(0.125L); 
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
  Num::init(62);
  return RUN_ALL_TESTS();
}