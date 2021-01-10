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
}  // namespace

unsigned Num::_negPowerOf2 = 0;
Num *Num::_lowest = NULL;
Num *Num::_min = NULL;
Num *Num::_max = NULL;

std::ostream &operator<<(std::ostream &out, const Num &n) {
  return out << string(n);
}

__int128 Num::parse(const string &s) {
  P(s);
  string decimal;
  string fraction;
  string exponent;
  splitNumber(s, &decimal, &fraction, &exponent);

  string mantissaS = decimal + fraction;
  unsigned negExponent10 = -atoi(exponent.c_str()) + fraction.length();
  P(negExponent10);
  __int128 mantissa = 0;
  for (char ch : mantissaS) {
    mantissa = (mantissa * 10) + (ch - '0');
    P((long long)mantissa);
  }
  unsigned closestNegPowerOf10 = llrintl(_negPowerOf2 * M_LN2l / M_LN10l) + 1;
  P(closestNegPowerOf10);
  while (negExponent10 < closestNegPowerOf10) {
    ++negExponent10;
    mantissa *= 10;
  }
  P(negExponent10);
  P((long long)mantissa);

  // long double scale = powl(2, _negPowerOf2 - negExponent10 * M_LN10l /
  // M_LN2l);
  long double scale = powl(2, _negPowerOf2) / powl(10, negExponent10);
  P(scale);
  return P(mantissa * scale);
}

Num::operator string() const {
  if (_mantissa == 0) {
    return "0";
  }
  unsigned negPowerOf10 = llrintl(_negPowerOf2 * M_LN2l / M_LN10l) + 1;

  long double mantissaAdj =
      _mantissa * powl(10, negPowerOf10) / powl(2, _negPowerOf2);
  string mantissaS = to_string(mantissaAdj);

  auto shift = [&mantissaS, &negPowerOf10](regex patt, string replace) {
    while (regex_match(mantissaS, patt) && negPowerOf10 > 0) {
      mantissaS = regex_replace(mantissaS, patt, replace);
      --negPowerOf10;
    }
  };
  auto simplify = [&mantissaS](const regex &patt, string replace) {
    while (regex_match(mantissaS, patt)) {
      mantissaS = regex_replace(mantissaS, patt, replace);
    }
  };

  simplify(trimToInteger, "$1");
  shift(canAddPoint, "$1.$2$3");
  simplify(trimTrailingFractionalZeros, "$1$2");
  shift(canMovePoint, "$1.$2$3");
  shift(canRemoveZero, "$1");
  simplify(trimToInteger, "$1");
  simplify(trimTrailingFractionalZeros, "$1$2");
  if (negPowerOf10 == 0) {
    return mantissaS;
  }
  return mantissaS + "e-" + to_string(negPowerOf10);
}

void Num::init(unsigned negPowerOf2) {
  assert(!_negPowerOf2);
  assert(negPowerOf2);
  _negPowerOf2 = negPowerOf2;
  _lowest = new Num(0);
  _min = new Num(0);
  _max = new Num(0);
  _lowest->_mantissa = numeric_limits<__int128>::lowest();
  _min->_mantissa = 1;
  _max->_mantissa = numeric_limits<__int128>::max();
  cout << "fixed::Num scaled to 2^-" << _negPowerOf2 << "\n"
       << " " << numeric_limits<__int128>::digits << " bits,"
       << " " << numeric_limits<__int128>::digits10 << " decimal digits\n"
       << " lowest = " << lowest() << "\n"
       << " min    = " << min() << "\n"
       << " max    = " << max() << endl;
}

}  // namespace fixed