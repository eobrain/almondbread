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

}  // namespace

unsigned Num::_negExponent = 0;
Num *Num::_lowest = NULL;
Num *Num::_min = NULL;
Num *Num::_max = NULL;
unsigned long long Num::_scale = 1;

std::ostream &operator<<(std::ostream &out, const Num &n) {
  return out << string(n);
}

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

long long Num::parse(const string &s) {
  string decimal;
  string fraction;
  string exponent;
  splitNumber(s, &decimal, &fraction, &exponent);

  string mantissaS = decimal + fraction;
  unsigned negExponent10 = -atoi(exponent.c_str()) + fraction.length();
  if (negExponent10 < _negExponent) {
    mantissaS += string(_negExponent - negExponent10, '0');
  } else if (negExponent10 > _negExponent) {
    mantissaS = mantissaS.substr(
        0, mantissaS.length() - (negExponent10 - _negExponent));
  }
  return atoll(mantissaS.c_str());
}

Num::operator string() const {
  return to_string(_mantissa) + "e-" + to_string(_negExponent);
}

void Num::init(unsigned negExponent) {
  assert(!_negExponent);
  assert(negExponent);
  _negExponent = negExponent;
  for (unsigned i = 0; i < _negExponent; ++i) {
    _scale *= 10;
  }
  _lowest = new Num(0);
  _min = new Num(0);
  _max = new Num(0);
  _lowest->_mantissa = numeric_limits<long long>::lowest();
  _min->_mantissa = 1;
  _max->_mantissa = numeric_limits<long long>::max();
  cout << "fixed::Num scaled to 10^-" << _negExponent << " = 1/" << _scale
       << "\n"
       << " " << numeric_limits<long long>::digits << " bits,"
       << " " << numeric_limits<long long>::digits10 << " decimal digits\n"
       << " lowest = " << lowest() << " = " << number(string(lowest())) << "\n"
       << " min    = " << min() << " = " << number(string(min())) << "\n"
       << " max    = " << max() << " = " << number(string(max())) << endl;
}

}  // namespace fixed