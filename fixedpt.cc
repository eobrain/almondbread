#include "fixedpt.h"

#include <limits>
#include <regex>
#include <stdexcept>

#ifndef NDEBUG
#define P(x)                                                           \
  (std::cout << __FILE__ << ":" << __LINE__ << ":" << #x << "=" << (x) \
             << std::endl,                                             \
   x)
#else
#define P(x)
#endif

namespace fixed {

using std::cout;
using std::endl;
using std::invalid_argument;
using std::numeric_limits;
using std::regex;
using std::regex_match;
using std::regex_replace;
using std::smatch;
using std::string;
using std::to_string;

namespace {
// (sign digits) "." (digits)  "e". (sign digits)
regex floatingLiteralE("^(\\-?[0-9]+)\\.([0-9]+)e(-?[0-9]+)$");
// (sign digits) "e". (sign digits)
regex integerLiteralE("^(\\-?[0-9]+)e(-?[0-9]+)$");
// (sign digits) "." (digits)
regex floatingLiteral("^(\\-?[0-9]+)\\.([0-9]+)$");
// (sign digits)
regex integerLiteral("^(\\-?[0-9]+)$");

void splitNumber(const string &s, string *decimal, string *fraction,
                 string *exponent) {
  smatch match;
  if (regex_match(s, match, floatingLiteralE)) {
    *decimal = match[1];
    *fraction = match[2];
    *exponent = match[3];
    return;
  }
  if (regex_match(s, match, integerLiteralE)) {
    *decimal = match[1];
    *fraction = "";
    *exponent = match[2];
    return;
  }
  if (regex_match(s, match, floatingLiteral)) {
    *decimal = match[1];
    *fraction = match[2];
    *exponent = "0";
    return;
  }
  if (regex_match(s, match, integerLiteral)) {
    *decimal = match[1];
    *fraction = "";
    *exponent = "0";
    return;
  }
  throw invalid_argument(s);
}

long double number(const string &s) { return strtold(s.c_str(), NULL); }

string toStringU128(unsigned __int128 x) {
  if (x < 10) return to_string((int)x);
  return toStringU128(x / 10) + to_string((int)(x % 10));
}

string toString128(__int128 x) {
  if (x < 0)
    return "-" +
           toStringU128((unsigned __int128)((~((unsigned __int128)x)) + 1));
  if (x < 10) return to_string((int)x);
  return toString128(x / 10) + to_string((int)(x % 10));
}

__int128 pow10(__int128 exp) {
  __int128 base = 10;
  __int128 result = 1;
  for (;;) {
    if (exp & 1) result *= base;
    exp >>= 1;
    if (!exp) break;
    base *= base;
  }

  return result;
}

__int128 num2int(Num n) {
  __int128 result;
  std::memcpy(&result, &n, 128 / 8);
  return result;
}

Num int2num(__int128 n) {
  Num result;
  std::memcpy(&result, &n, 128 / 8);
  return result;
}
}  // namespace

namespace impl {
unsigned negExponent = 0;
unsigned __int128 scale = 1;

}  // namespace impl

Num lowest;
Num min;
Num max;

std::ostream &operator<<(std::ostream &out, Num n) {
  return out << strtold(toString(n).c_str(), NULL);
}

string toString(Num a) {
  __int128 i = num2int(a);
  return toString128(i) + "e-" + to_string(impl::negExponent);
}

void init(unsigned negExponent) {
  assert(!impl::negExponent);
  assert(negExponent);
  impl::negExponent = negExponent;
  impl::scale = pow10(impl::negExponent);
  lowest = static_cast<Num>(numeric_limits<__int128>::lowest());
  min = static_cast<Num>(1);
  max = static_cast<Num>(numeric_limits<__int128>::max());
  cout << "fixed::Num scaled to 10^-" << impl::negExponent << " = 1/"
       << toStringU128(impl::scale) << "\n"
       << " " << numeric_limits<__int128>::digits << " bits,"
       << " " << numeric_limits<__int128>::digits10 << " decimal digits\n"
       << " lowest = " << lowest << " = " << number(toString(lowest)) << "\n"
       << " min    = " << min << " = " << number(toString(min)) << "\n"
       << " max    = " << max << " = " << number(toString(max)) << endl;
}

__int128 parseInt128(const string &s) {
  __int128 result = 0;
  for (char ch : s) {
    result *= 10;
    result += ch - '0';
  }
  return result;
}

Num parse(const string &s) {
  string decimal;
  string fraction;
  string exponent;
  splitNumber(s, &decimal, &fraction, &exponent);

  string mantissaS = decimal + fraction;
  unsigned negExponent10 = -atoi(exponent.c_str()) + fraction.length();
  if (negExponent10 < impl::negExponent) {
    mantissaS += string(impl::negExponent - negExponent10, '0');
  } else if (negExponent10 > impl::negExponent) {
    mantissaS = mantissaS.substr(
        0, mantissaS.length() - (negExponent10 - impl::negExponent));
  }
  return static_cast<Num>(parseInt128(mantissaS));
}

}  // namespace fixed