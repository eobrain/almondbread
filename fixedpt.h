#ifndef NUM_H
#define NUM_H

#include <assert.h>
#include <math.h>

#include <iostream>
#include <string>
#include <utility>

namespace fixed {

enum Num : __int128 { f0 = 0 };

namespace impl {
extern unsigned negExponent;
extern unsigned __int128 scale;
}  // namespace impl
extern void init(unsigned negExponent);
extern Num lowest;
extern Num min;
extern Num max;
inline Num toNum(__int128 value) {
  return static_cast<Num>(value * impl::scale);
}
inline Num toNum(int value) { return static_cast<Num>(value * impl::scale); }
inline Num toNum(long double value) {
  return static_cast<Num>(llrintl(value * impl::scale));
}
inline Num toNum(double value) {
  return static_cast<Num>(llrint(value * pow(10, impl::negExponent)));
}
extern Num parse(const std::string &s);
extern std::string toString(Num a);

template <typename T>
inline Num operator+(Num a, T b) {
  return static_cast<Num>(static_cast<__int128>(a) + b * impl::scale);
}
template <>
inline Num operator+(Num a, Num b) {
  return static_cast<Num>(static_cast<__int128>(a) + static_cast<__int128>(b));
}

template <typename T>
inline Num operator-(Num a, T b) {
  return static_cast<Num>(static_cast<__int128>(a) - b * impl::scale);
}
template <>
inline Num operator-(Num a, Num b) {
  return static_cast<Num>(static_cast<__int128>(a) - static_cast<__int128>(b));
}

template <typename T>
inline Num operator*(Num a, T b) {
  return static_cast<Num>(static_cast<__int128>(a) * b);
}
template <>
inline Num operator*(Num a, Num b) {
  return static_cast<Num>((long double)static_cast<__int128>(a) *
                          static_cast<__int128>(b) / impl::scale);
}

template <typename T>
inline Num operator/(Num a, T b) {
  return static_cast<Num>(static_cast<__int128>(a) / b);
}
template <>
inline Num operator/(Num a, Num b) {
  return static_cast<Num>(impl::scale * static_cast<__int128>(a) /
                          static_cast<__int128>(b));
}

template <typename T>
inline bool operator>(Num a, T b) {
  return static_cast<__int128>(a) > b * impl::scale;
}
template <>
inline bool operator>(Num a, Num b) {
  return static_cast<__int128>(a) > static_cast<__int128>(b);
}
extern std::ostream &operator<<(std::ostream &, Num);

}  // namespace fixed

#endif  // NUM_H
