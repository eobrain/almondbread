#ifndef NUM_H
#define NUM_H

#include <assert.h>
#include <math.h>

#include <iostream>
#include <string>
#include <utility>

namespace fixed {

enum Num : long long { f0 = 0 };

namespace impl {
extern unsigned negExponent;
extern unsigned long long scale;
}  // namespace impl
extern void init(unsigned negExponent);
extern Num lowest;
extern Num min;
extern Num max;
inline Num toNum(long long value) { return Num(value * impl::scale); }
inline Num toNum(int value) { return Num(value * impl::scale); }
inline Num toNum(long double value) {
  return Num(llrintl(value * impl::scale));
}
inline Num toNum(double value) {
  return Num(llrint(value * pow(10, impl::negExponent)));
}
extern Num parse(const std::string &s);
extern std::string toString(Num a);

template <typename T>
inline Num operator+(Num a, T b) {
  return Num((long long)a + b * impl::scale);
}
template <>
inline Num operator+(Num a, Num b) {
  return Num((long long)a + (long long)b);
}

template <typename T>
inline Num operator-(Num a, T b) {
  return Num((long long)a - b * impl::scale);
}
template <>
inline Num operator-(Num a, Num b) {
  return Num((long long)a - (long long)b);
}

template <typename T>
inline Num operator*(Num a, T b) {
  return Num((long long)a * b);
}
template <>
inline Num operator*(Num a, Num b) {
  return Num((long double)(long long)a * (long long)b / impl::scale);
}

template <typename T>
inline Num operator/(Num a, T b) {
  return Num((long long)a / b);
}
template <>
inline Num operator/(Num a, Num b) {
  return Num(impl::scale * (long long)a / (long long)b);
}

template <typename T>
inline bool operator>(Num a, T b) {
  return (long long)a > b * impl::scale;
}
template <>
inline bool operator>(Num a, Num b) {
  return (long long)a > (long long)b;
}
extern std::ostream &operator<<(std::ostream &, Num);

}  // namespace fixed

#endif  // NUM_H
