#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include <array>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <map>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <vector>

#include "fixedpt.h"
#include "lodepng.h"

using fixed::f0;
using fixed::init;
using fixed::Num;
using fixed::parse;
using fixed::toNum;
using std::array;
using std::cerr;
using std::cout;
using std::endl;
using std::flush;
using std::getenv;
using std::map;
using std::numeric_limits;
using std::ofstream;
using std::ostream;
using std::string;
using std::stringstream;
using std::thread;
using std::unordered_map;
using std::vector;

#ifndef NDEBUG
#define P(x) \
  cout << __FILE__ << ":" << __LINE__ << ":" << #x << "=" << x << endl
#else
#define P(x)
#endif

namespace {

constexpr int INT_MIN = numeric_limits<int>::min();
constexpr int INT_MAX = numeric_limits<int>::max();

struct Params {
  int imgWidth = 1400;
  int imgHeight = 900;
  Num centerRe = toNum(-0.5671L);
  Num centerIm = toNum(-0.56698L);
  Num width = toNum(0.2L);
  int maxIterationCount = 10000;
  const char *outputFileName = "mandelbrot.png";
};
ostream &operator<<(ostream &out, const Params &p) {
  out << "imgWidth=" << p.imgWidth << " imgHeight=" << p.imgHeight
      << " centerRe=" << p.centerRe << " centerIm=" << p.centerIm
      << " width=" << p.width << " maxIterationCount=" << p.maxIterationCount
      << " outputFileName=" << p.outputFileName;
  return out;
}

class Stats {
  map<int, int> _histogram;
  unordered_map<int, double> _itersToPercentile;

  int _totalCount = 0;
  int _min = INT_MAX;
  int _max = INT_MIN;

 public:
  void operator()(int i) {
    if (i > _max) _max = i;
    if (i < _min) _min = i;
    auto lookup = _histogram.find(i);
    int prev = lookup == _histogram.end() ? 0 : lookup->second;
    _histogram[i] = prev + 1;
    ++_totalCount;
  }
  void preparePercentile() {
    double acc = 0;
    for (auto const &pair : _histogram) {
      _itersToPercentile[pair.first] = (acc + pair.second / 2) / _totalCount;
      acc += pair.second;
    }
  }
  double normalize(int i) const { return 1.0 * (i - _min) / (_max - _min); }
  double percentile(int i) const { return _itersToPercentile.at(i); }
  int mapped(int iterations) const { return iterations - _min; }
  int range() const { return _max - _min; }
  friend ostream &operator<<(ostream &, const Stats &);
};
ostream &operator<<(ostream &out, const Stats &s) {
  out << "Iterations,Count\n";
  for (int i = s._min; i <= s._max; ++i) {
    auto lookup = s._histogram.find(i);
    if (lookup != s._histogram.end()) {
      out << i << "," << lookup->second << "\n";
    }
  }
  return out;
}

void addText(lodepng::State *state, const char *key, const string &value) {
  unsigned err = lodepng_add_text(&state->info_png, key, value.c_str());
  if (err) throw err;
}

class Image {
  const int _width;
  const int _height;
  vector<int> _iterations;
  unsigned char *const _pixels;

 public:
  Image(int width, int height)
      : _width(width),
        _height(height),
        _iterations(width * height),
        _pixels(new unsigned char[width * height * 4]) {}
  ~Image() { delete[] _pixels; }
  int &iterations(int ix, int iy) {
    // Optimized for ix changing faster
    return _iterations[ix + _width * iy];
  }

  unsigned char &pixel(int ix, int iy, int layer) {
    return _pixels[4 * ix + 4 * _width * iy + layer];
  }

  int centerIterations() { return iterations(_width / 2, _height / 2); }

  void stretchColor() {
    int n = _width * _height * 4;
    int maxs[] = {0, 0, 0};
    for (int i = 0; i < n; ++i) {
      int layer = i % 4;
      if (layer < 3) {
        if (_pixels[i] > maxs[layer]) {
          maxs[layer] = _pixels[i];
        }
      }
    }
    cout << "maxs=" << maxs[0] << " " << maxs[1] << " " << maxs[1] << endl;
    for (int i = 0; i < n; ++i) {
      int layer = i % 4;
      if (layer < 3 && maxs[layer] > 0) {
        _pixels[i] = (int)_pixels[i] * 256 / maxs[layer];
      }
    }
  }

  bool writePng(const Params &params) const {
    try {
      lodepng::State state;
      lodepng_info_init(&state.info_png);

      stringstream titleStream;
      titleStream << "Mandelbrot Set At (" << params.centerRe << ","
                  << params.centerIm << ")";
      const string title = titleStream.str();
      addText(&state, "Title", title);

      const string author = getenv("USER");
      addText(&state, "Author", author);

      stringstream descriptionStream;
      descriptionStream << "\n\nThis is a view of the Mandelbrot set that is "
                        << params.width
                        << " wide,\ncalculated with a maximum of "
                        << params.maxIterationCount << " iterations per pixel.";
      const string description = descriptionStream.str();
      addText(&state, "Description", description);

      // TODO(eob) Allow creator name to be parameterized to be someone other
      // than me.
      const string copyright =
          " by Eamonn O'Brien-Strain, licensed under CC "
          "BY-NC-SA 4.0. To view a copy of this license, visit "
          "https://creativecommons.org/licenses/by-nc-sa/4.0";
      addText(&state, "Copyright", title + " " + copyright);

      const string software =
          "Almond Bread (https://github.com/eobrain/almondbread)";
      addText(&state, "Software", software);

      char hostname[100];
      if (gethostname(hostname, (sizeof hostname) - 1) != 0) {
        strcpy(hostname, "(unknown host}");
      }
      addText(&state, "Source", hostname);

      addText(&state, "Comment",
              title + "\nCopyright " + copyright + "\nCreated by " + author +
                  "@" + hostname + " using " + software + ".\n" + description);

      vector<unsigned char> png;
      unsigned err = lodepng::encode(png, _pixels, params.imgWidth,
                                     params.imgHeight, state);
      if (err) throw err;

      lodepng::save_file(png, params.outputFileName);
    } catch (unsigned err) {
      cerr << "encoder error " << err << ": " << lodepng_error_text(err)
           << endl;
      return false;
    }
    return true;
  }
};

array<double, 3> hsv2rgb(double h, double s, double v) {
  double c = v * s;
  double x = c * (1.0 - fabs(fmod(h / 60.0, 2) - 1.0));
  double m = v - c;
  if (h >= 0.0 && h < 60.0) {
    return {c + m, x + m, m};
  }
  if (h >= 60.0 && h < 120.0) {
    return {x + m, c + m, m};
  }
  if (h >= 120.0 && h < 180.0) {
    return {m, c + m, x + m};
  }
  if (h >= 180.0 && h < 240.0) {
    return {m, x + m, c + m};
  }
  if (h >= 240.0 && h < 300.0) {
    return {x + m, m, c + m};
  }
  if (h >= 300.0 && h < 360.0) {
    return {c + m, m, x + m};
  }
  return {m, m, m};
}

constexpr unsigned char clamp(int color) { return color >= 255 ? 255 : color; }

int iterations(int maxIterationCount, Num cRe, Num cIm) {
  Num zRe = f0;
  Num zIm = f0;
  Num zRe2 = f0;
  Num zIm2 = f0;
  for (int i = 0; i < maxIterationCount; ++i) {
    Num zReNew = zRe2 - zIm2 + cRe;
    Num zImNew = zRe * zIm * 2 + cIm;
    zRe2 = zReNew * zReNew;
    zIm2 = zImNew * zImNew;
    if (zRe2 + zIm2 > 4) {
      return i;
    }
    zRe = zReNew;
    zIm = zImNew;
  }
  return maxIterationCount;
}

int iterations(const Params &params, int ix, int iy) {
  Num scale = params.width / params.imgWidth;
  Num cRe = scale * (ix - params.imgWidth / 2) + params.centerRe;
  Num cIm = scale * (params.imgHeight / 2 - iy) + params.centerIm;
  return iterations(params.maxIterationCount, cRe, cIm);
}

const int COLOR_SCALE = 10;

void setColor(const Stats &stats, Image *img, int maxIterationCount, int ix,
              int iy) {
  int iters = img->iterations(ix, iy);
  if (iters == maxIterationCount) {
    img->pixel(ix, iy, 0) = 0;
    img->pixel(ix, iy, 1) = 0;
    img->pixel(ix, iy, 2) = 0;
    img->pixel(ix, iy, 3) = 255;
    return;
  }

  double value = stats.percentile(iters);
  value *= value;
  auto rgb = hsv2rgb(250 * value, value, 1 - value / 2);
  for (int i = 0; i < 3; ++i) {
    img->pixel(ix, iy, i) = rgb[i] * 256;
  }
  img->pixel(ix, iy, 3) = 255;
}

int threadCount = thread::hardware_concurrency();

void threadWorker(const Params &params, Image *img, int mod) {
  for (int iy = mod; iy < params.imgHeight; iy += threadCount)
    for (int ix = 0; ix < params.imgWidth; ++ix) {
      img->iterations(ix, iy) = iterations(params, ix, iy);
    }
  cout << "Finished thread " << mod << endl;
}

}  // namespace

int main(int argc, char *const argv[]) {
  P(threadCount);
  init(35);
  Params params;

  int opt;
  while ((opt = getopt(argc, argv, "W:H:x:y:w:i:o:")) != -1) {
    switch (opt) {
      case 'W':
        params.imgWidth = atoi(optarg);
        break;
      case 'H':
        params.imgHeight = atoi(optarg);
        break;
      case 'x':
        params.centerRe = parse(optarg);
        break;
      case 'y':
        params.centerIm = parse(optarg);
        break;
      case 'w':
        params.width = parse(optarg);
        break;
      case 'i':
        params.maxIterationCount = atoi(optarg);
        break;
      case 'o':
        params.outputFileName = optarg;
        break;
      default: /* '?' */
        cerr << "Usage: " << argv[0]
             << " -W imgWidth -H imgHeight -x centerReal -y centerImaginary -w "
                "viewportWidth -i iterations"
             << endl
             << "  defaults:  -W 1400 -H 900  -x -0.5671 -y -0.56698 -w 0.2 -i "
                "10000 -o mandelbrot.png"
             << endl;
        return EXIT_FAILURE;
    }
  }

  P(params);

  Image img(params.imgWidth, params.imgHeight);
  Stats stats;
  vector<thread> threads;
  for (int mod = 0; mod < threadCount; ++mod) {
    threads.emplace_back(threadWorker, params, &img, mod);
  }
  for (auto &thread : threads) {
    thread.join();
  }

  for (int iy = 0; iy < params.imgHeight; ++iy)
    for (int ix = 0; ix < params.imgWidth; ++ix) {
      int iters = img.iterations(ix, iy);
      if (iters != params.maxIterationCount) {
        stats(iters);
      }
    }
  stats.preparePercentile();
  for (int iy = 0; iy < params.imgHeight; ++iy)
    for (int ix = 0; ix < params.imgWidth; ++ix) {
      setColor(stats, &img, params.maxIterationCount, ix, iy);
    }

  // img.stretchColor();

  P(params);
  bool ok = img.writePng(params);

  ofstream histogram("histogram.csv");
  histogram << stats;
  histogram.close();

  return ok ? 0 : 1;
}
