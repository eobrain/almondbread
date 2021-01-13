#ifndef NUM_H
#define NUM_H

#include <assert.h>
#include <math.h>

#include <iostream>
#include <string>
#include <utility>

namespace fixed {

class Num {
  static unsigned _negExponent;
  static unsigned long long _scale;
  static Num *_lowest;
  static Num *_min;
  static Num *_max;
  long long _mantissa;
  static long long mantissa(long long value) { return value * _scale; }
  static long long mantissa(int value) { return value * _scale; }
  static long long mantissa(long double value) {
    return llrintl(value * _scale);
  }
  static long long mantissa(double value) {
    return llrint(value * pow(10, _negExponent));
  }
  static long long parse(const std::string &s);

 public:
  static void init(unsigned negExponent);
  static const Num &lowest() { return *_lowest; }
  static const Num &min() { return *_min; }
  static const Num &max() { return *_max; }
  Num(const Num &b) : _mantissa(b._mantissa) {}
  // Num(const Num &&b) : _mantissa(std::exchange(b._mantissa, 0LL)) {}
  Num(const Num &&b) : _mantissa(b._mantissa) {}
  Num(int value) : _mantissa(mantissa(value)) { assert(_negExponent); }
  Num(long double value) : _mantissa(mantissa(value)) { assert(_negExponent); }
  Num(double value) : _mantissa(mantissa(value)) { assert(_negExponent); }
  Num(const std::string &s) : _mantissa(parse(s)) { assert(_negExponent); }
  operator std::string() const;
  Num &operator=(const Num &other) {
    _mantissa = other._mantissa;
    return *this;
  }
  Num &operator=(Num &&other) {
    _mantissa = std::move(other._mantissa);
    return *this;
  }
  Num &operator+=(const Num &b) {
    _mantissa += b._mantissa;
    return *this;
  };
  Num &operator+=(long long b) {
    _mantissa += mantissa(b);
    return *this;
  };
  Num &operator-=(const Num &b) {
    _mantissa -= b._mantissa;
    return *this;
  };
  Num &operator-=(long long b) {
    _mantissa -= mantissa(b);
    return *this;
  };
  Num &operator*=(const Num &b) {
    _mantissa = (long double)_mantissa * b._mantissa / _scale;
    return *this;
  };
  Num &operator*=(long long b) {
    _mantissa *= b;
    return *this;
  };
  Num &operator*=(int b) {
    _mantissa *= b;
    return *this;
  };
  Num &operator/=(long long b) {
    _mantissa /= b;
    return *this;
  };
  Num &operator/=(int b) {
    _mantissa /= b;
    return *this;
  };
  Num &negate() {
    _mantissa = -_mantissa;
    return *this;
  }
  Num operator-() { return Num(*this).negate(); }

  bool operator>(long long b) const { return _mantissa > mantissa(b); }
};
inline Num operator+(int a, const Num &b) { return Num(b) += a; }
template <typename T>
inline Num operator+(const Num &a, const T &b) {
  return Num(a) += b;
}
template <typename T>
inline Num operator-(const Num &a, const T &b) {
  return Num(a) -= b;
}
inline Num operator-(int a, const Num &b) { return b - a; }
template <typename T>
inline Num operator*(const Num &a, const T &b) {
  return Num(a) *= b;
}
template <typename T>
inline Num operator/(const Num &a, T b) {
  return Num(a) /= b;
}
extern std::ostream &operator<<(std::ostream &out, const Num &s);

}  // namespace fixed

#endif  // NUM_H
