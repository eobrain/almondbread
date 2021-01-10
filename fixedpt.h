#ifndef NUM_H
#define NUM_H

#include <assert.h>
#include <math.h>

#include <iostream>
#include <string>
#include <utility>

namespace fixed {

class Num {
  static unsigned long long _negPowerOf2;
  static  Num *_lowest;
  static  Num *_min;
  static  Num *_max;
  long long _mantissa;
  static long long mantissa(long long value) { return value << _negPowerOf2; }
  static long long mantissa(long double value) {
    return llrintl(value * (1 << _negPowerOf2));
  }
  static long long parse(const std::string &s);

 public:
  static void init(unsigned long long negPowerOf2);
  static const Num &lowest() { return *_lowest; }
  static const Num &min() { return *_min; }
  static const Num &max() { return *_max; }
  // Num(const Num &b) : _mantissa(b._mantissa) {}
  // Num(const Num &&b) : _mantissa(std::exchange(b._mantissa, 0LL)) {}
  Num(long long value) : _mantissa(mantissa(value)) { assert(_negPowerOf2); }
  Num(long double value) : _mantissa(mantissa(value)) { assert(_negPowerOf2); }
  Num(const std::string &s) : _mantissa(parse(s)) { assert(_negPowerOf2); }
  operator std::string() const;
  // friend Num operator*(long long a, const Num &b);
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
    _mantissa *= b._mantissa;
    _mantissa >>= _negPowerOf2;
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
inline Num operator+(const Num &a, const Num &b) { return Num(a) += b; }
inline Num operator-(const Num &a, const Num &b) { return Num(a) -= b; }
inline Num operator-(int a, const Num &b) { return -(Num(b) -= a); }
inline Num operator-(const Num &a, int b) { return Num(a) -= b; }
inline Num operator*(const Num &a, const Num &b) { return Num(a) *= b; }
inline Num operator*(long long a, const Num &b) { return Num(b) *= a; }
inline Num operator*(const Num &a, long long b) { return Num(a) *= b; }
inline Num operator*(const Num &a, int b) { return Num(a) *= b; }
inline Num operator/(const Num &a, long long b) { return Num(a) /= b; }
inline Num operator/(const Num &a, int b) { return Num(a) /= b; }
extern std::ostream &operator<<(std::ostream &out, const Num &s);

}  // namespace fixed

#endif  // NUM_H
