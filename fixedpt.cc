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
regex trimToInteger("^(.*)\\.0+$");
regex trimTrailingFractionalZeros("^(.*)(\\..*[^0])0+$");
regex canAddPoint("^([^\1.]+)([^\1.])$");
regex canMovePoint("^(.+)(.)\\.(.+)$");
regex canRemoveZero("^([^\\.]+)0$");
regex floatingLiteralE("^(\\-?[0-9]+)\\.([0-9]+)e([+-]?[0-9]+)$");
regex floatingLiteral("^(\\-?[0-9]+)\\.([0-9]+)$");
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

string toString(__int128 i) {
  string sign = "";
  if (i < 0) {
    sign = "-";
    i = -i;
  }
  P(sign);
  P((long long)(i >> 64));
  P((long long)i);
  if (i == 0) {
    return "0";
  }
  string result;
  while (i > 0) {
    result = string(1, '0' + char(i % 10)) + result;
    i /= 10;
  }
  P(result);
  return sign + result;
}

}  // namespace

unsigned Num::_negExponent = 0;
Num *Num::_lowest = NULL;
Num *Num::_min = NULL;
Num *Num::_max = NULL;
__int128 Num::_scale = 1;

std::ostream &operator<<(std::ostream &out, const Num &n) {
  return out << string(n);
}

__int128 Num::parse(const string &s) {
  string decimal;
  string fraction;
  string exponent;
  splitNumber(s, &decimal, &fraction, &exponent);

  string mantissaS = decimal + fraction;
  unsigned negExponent10 = -atoi(exponent.c_str()) + fraction.length();
  __int128 mantissa = 0;
  for (char ch : mantissaS) {
    mantissa = (mantissa * 10) + (ch - '0');
  }
  while (negExponent10 > _negExponent) {
    --negExponent10;
    mantissa /= 10;
  }
  while (negExponent10 < _negExponent) {
    ++negExponent10;
    mantissa *= 10;
  }
  return mantissa;
}

Num::operator string() const {
  if (_mantissa == 0) {
    return "0";
  }
  int powerOf10 = -_negExponent;

  string mantissaS = toString(_mantissa);
  unsigned n = mantissaS.length();
  P(mantissaS);
  if (n > 1) {
    powerOf10 += n - 1;
    mantissaS =
        mantissaS.substr(0, 1) + '.' + mantissaS.substr(1, string::npos);
    P(mantissaS);
  }
  mantissaS = regex_replace(mantissaS, trimTrailingFractionalZeros, "$1$2");
  mantissaS = regex_replace(mantissaS, trimToInteger, "$1");
  if (powerOf10 == 0) {
    return mantissaS;
  }
  return mantissaS + "e" + to_string(powerOf10);
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
  _lowest->_mantissa = numeric_limits<__int128>::lowest();
  _min->_mantissa = 1;
  _max->_mantissa = numeric_limits<__int128>::max();
  cout << "fixed::Num scaled to 2^-" << _negExponent << "\n"
       << " " << numeric_limits<__int128>::digits << " bits,"
       << " " << numeric_limits<__int128>::digits10 << " decimal digits\n"
       << " lowest = " << lowest() << "\n"
       << " min    = " << min() << "\n"
       << " max    = " << max() << endl;
}

}  // namespace fixed