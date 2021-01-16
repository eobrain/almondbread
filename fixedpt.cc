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

long long pow10(long long exp) {
  long long base = 10;
  long long result = 1;
  for (;;) {
    if (exp & 1) result *= base;
    exp >>= 1;
    if (!exp) break;
    base *= base;
  }

  return result;
}
}  // namespace

namespace impl {
unsigned negExponent = 0;
unsigned long long scale = 1;

}  // namespace impl

Num lowest;
Num min;
Num max;

std::ostream &operator<<(std::ostream &out, Num n) {
  return out << strtold(toString(n).c_str(), NULL);
}

string toString(Num a) {
  return to_string((long long)a) + "e-" + to_string(impl::negExponent);
}

void init(unsigned negExponent) {
  assert(!impl::negExponent);
  assert(negExponent);
  impl::negExponent = negExponent;
  impl::scale = pow10(impl::negExponent);
  lowest = static_cast<Num>(numeric_limits<long long>::lowest());
  min = static_cast<Num>(1);
  max = static_cast<Num>(numeric_limits<long long>::max());
  cout << "fixed::Num scaled to 10^-" << impl::negExponent << " = 1/"
       << impl::scale << "\n"
       << " " << numeric_limits<long long>::digits << " bits,"
       << " " << numeric_limits<long long>::digits10 << " decimal digits\n"
       << " lowest = " << lowest << " = " << number(toString(lowest)) << "\n"
       << " min    = " << min << " = " << number(toString(min)) << "\n"
       << " max    = " << max << " = " << number(toString(max)) << endl;
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
  return static_cast<Num>(atoll(mantissaS.c_str()));
}

}  // namespace fixed