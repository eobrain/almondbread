#include "fixedpt.h"

#include <limits>
#include <regex>

#ifndef NDEBUG
#define P(x) \
  std::cout << __FILE__ << ":" << __LINE__ << ":" << #x << "=" << x << std::endl
#else
#define P(x)
#endif

namespace fixed {

using std::cout;
using std::endl;
using std::numeric_limits;
using std::regex;
using std::regex_match;
using std::regex_replace;
using std::string;
using std::to_string;

namespace {
regex trimToInteger("^(.*)\\.0+$");
regex trimTrailingFractionalZeros("^(.*)(\\..*[^0])0+$");
regex canAddPoint("^([^\1.]+)([^\1.])$");
regex canMovePoint("^(.+)(.)\\.(.+)$");
regex canRemoveZero("^([^\\.]+)0$");
}  // namespace

unsigned long long Num::_negPowerOf2 = 0;
Num *Num::_lowest = NULL;
Num *Num::_min = NULL;
Num *Num::_max = NULL;

std::ostream &operator<<(std::ostream &out, const Num &n) {
  return out << string(n);
}

long long Num::parse(const string &s) {
  P(s);
  size_t sepPos = s.find("e");
  P(sepPos);
  if (sepPos == string::npos) {
    return atoll(s.c_str()) << _negPowerOf2;
  }
  unsigned long long mantissa = atoll(s.substr(0, sepPos).c_str());
  P(mantissa);
  unsigned long long exponent10 =
      atoll(s.substr(sepPos + 1, string::npos).c_str());
  P(exponent10);
  long double scale = powl(2, _negPowerOf2 - exponent10);
  P(scale);
  return mantissa * scale;
}

Num::operator string() const {
  if (_mantissa == 0) {
    return "0";
  }
  long long unsigned negPowerOf10 =
      llrintl(_negPowerOf2 * M_LN2l / M_LN10l) + 1;

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

void Num::init(unsigned long long negPowerOf2) {
  assert(!_negPowerOf2);
  assert(negPowerOf2);
  _negPowerOf2 = negPowerOf2;
  _lowest = new Num(0LL);
  _min = new Num(0LL);
  _max = new Num(0LL);
  _lowest->_mantissa = numeric_limits<long long>::lowest();
  _min->_mantissa = 1;
  _max->_mantissa = numeric_limits<long long>::max();
  cout << "fixed::Num scaled to 2^-" << _negPowerOf2 << "\n"
       << " " << numeric_limits<long long>::digits << " bits\n"
       << " " << numeric_limits<long long>::digits10 << " decimal digits\n"
       << " lowest= " << lowest() << "\n"
       << " min   = " << min() << "\n"
       << " max   = " << max() << endl;
}

}  // namespace fixed