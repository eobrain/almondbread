#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <vector>

#include "lodepng.h"

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
  (cout << __FILE__ << ":" << __LINE__ << ":" << #x << "=" << x << endl, x)
#else
#define P(x) x
#endif

namespace {

constexpr int INT_MIN = numeric_limits<int>::min();
constexpr int INT_MAX = numeric_limits<int>::max();

template <typename T>
struct Params {
  int HD_IMG_WIDTH = 1400;
  int HD_IMG_HEIGHT = 900;
  T centerRe = -0.5671;
  T centerIm = -0.56698;
  T width = 0.2;
  int maxIterationCount = 10000;
  const char *outputFileName = "mandelbrot.png";
};
template <typename T>
ostream &operator<<(ostream &out, const Params<T> &p) {
  out << "HD_IMG_WIDTH=" << p.HD_IMG_WIDTH
      << " HD_IMG_HEIGHT=" << p.HD_IMG_HEIGHT << " centerRe=" << p.centerRe
      << " centerIm=" << p.centerIm << " width=" << p.width
      << " maxIterationCount=" << p.maxIterationCount
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
  cout << "addText(,\"" << key << "\", \"" << value << "\")" << endl;
  unsigned err = lodepng_add_text(&state->info_png, key, value.c_str());
  if (err) throw err;
}

// hill-shading parameters
constexpr double Z_FACTOR = 1;
constexpr double KERNELSIZE = 1;
constexpr double ALTITUDE = 45 * M_PI / 180;
constexpr double AZIMUTH = 135 * M_PI / 180;

// Derived hill-shading values
constexpr double ZENITH = M_PI / 2 - ALTITUDE;

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
  // Optimized for ix changing faster
  int &iterations(int ix, int iy) { return _iterations[ix + _width * iy]; }

  unsigned char &pixel(int ix, int iy, int layer) {
    return _pixels[4 * ix + 4 * _width * iy + layer];
  }

  int centerIterations() { return iterations(_width / 2, _height / 2); }

  /** https://pro.arcgis.com/en/pro-app/latest/tool-reference/3d-analyst/how-hillshade-works.htm
   * https://blog.datawrapper.de/shaded-relief-with-gdal-python/
   */
  double hillshade(int ix, int iy) {
    // Values in the eight neighboring cells
    const double a = iterations(ix - 1, iy - 1);
    const double b = iterations(ix, iy - 1);
    const double c = iterations(ix + 1, iy - 1);
    const double d = iterations(ix - 1, iy);
    const double f = iterations(ix + 1, iy);
    const double g = iterations(ix - 1, iy + 1);
    const double h = iterations(ix, iy + 1);
    const double i = iterations(ix + 1, iy + 1);

    const double dzdx = ((c + 2 * f + i) - (a + 2 * d + g)) / (8 * KERNELSIZE);
    const double dzdy = ((g + 2 * h + i) - (a + 2 * b + c)) / (8 * KERNELSIZE);

    const double slope = atan(Z_FACTOR * sqrt(dzdx * dzdx + dzdy * dzdy));

    const double aspect = atan2(dzdy, -dzdx);
    double shade = ((cos(ZENITH) * cos(slope)) +
                    (sin(ZENITH) * sin(slope) * cos(AZIMUTH - aspect)));
    return shade < 0 ? 0 : shade;
  }

  template <typename T>
  bool writePng(const Params<T> &params) const {
    try {
      lodepng::State state;
      lodepng_info_init(&state.info_png);

      stringstream titleStream;
      titleStream << "Mandelbrot Set At (" << params.centerRe << ","
                  << params.centerIm << ")";
      const string title = titleStream.str();
      addText(&state, "Title", title);

      const char *maybeAuthor = getenv("USER");
      if (maybeAuthor) {
        const string author = maybeAuthor;
        addText(&state, "Author", author);
      }

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

      if (maybeAuthor) {
        addText(&state, "Comment",
                title + "\nCopyright " + copyright + "\nCreated by " +
                    maybeAuthor + "@" + hostname + " using " + software +
                    ".\n" + description);
      } else {
        addText(&state, "Comment",
                title + "\nCopyright " + copyright + "\nCreated on " +
                    hostname + " using " + software + ".\n" + description);
      }

      vector<unsigned char> png;
      unsigned err = lodepng::encode(png, _pixels, params.HD_IMG_WIDTH,
                                     params.HD_IMG_HEIGHT, state);
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

template <typename T>
int iterations(int maxIterationCount, T cRe, T cIm) {
  T zRe = 0;
  T zIm = 0;
  T zRe2 = 0;
  T zIm2 = 0;
  for (int i = 0; i < maxIterationCount; ++i) {
    T zReNew = zRe2 - zIm2 + cRe;
    T zImNew = 2 * zRe * zIm + cIm;
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

template <typename T>
int iterations(const Params<T> &params, int ix, int iy) {
  T scale = params.width / params.HD_IMG_WIDTH;
  T cRe = scale * (ix - params.HD_IMG_WIDTH / 2) + params.centerRe;
  T cIm = scale * (params.HD_IMG_HEIGHT / 2 - iy) + params.centerIm;
  return iterations(params.maxIterationCount, cRe, cIm);
}

const int COLOR_SCALE = 10;

void setColor(const Stats &stats, Image *img, int maxIterationCount, int ix,
              int iy, double shade) {
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
  auto rgb = hsv2rgb(250 * value, (1 - 3 * value / 4) / 2, (2 + shade) / 3);
  for (int i = 0; i < 3; ++i) {
    img->pixel(ix, iy, i) = rgb[i] * 256;
  }
  img->pixel(ix, iy, 3) = 255;
}

int threadCount = thread::hardware_concurrency();

template <typename T>
void threadWorker(const Params<T> &params, Image *img, int mod) {
  for (int iy = mod; iy < params.HD_IMG_HEIGHT; iy += threadCount)
    for (int ix = 0; ix < params.HD_IMG_WIDTH; ++ix) {
      img->iterations(ix, iy) = iterations(params, ix, iy);
    }
  cout << "Finished thread " << mod << endl;
}

}  // namespace

int main(int argc, char *const argv[]) {
  Params<long double> params;

  int opt;
  while ((opt = getopt(argc, argv, "W:H:x:y:w:i:o:")) != -1) {
    switch (opt) {
      case 'W':
        params.HD_IMG_WIDTH = atoi(optarg);
        break;
      case 'H':
        params.HD_IMG_HEIGHT = atoi(optarg);
        break;
      case 'x':
        params.centerRe = strtold(optarg, NULL);
        break;
      case 'y':
        params.centerIm = strtold(optarg, NULL);
        break;
      case 'w':
        params.width = strtold(optarg, NULL);
        break;
      case 'i':
        params.maxIterationCount = atoi(optarg);
        break;
      case 'o':
        params.outputFileName = optarg;
        break;
      default: /* '?' */
        cerr << "Usage: " << argv[0]
             << " -W HD_IMG_WIDTH -H HD_IMG_HEIGHT -x centerReal -y "
                "centerImaginary "
                "-w "
                "viewportWidth -i iterations"
             << endl
             << "  defaults:  -W 1400 -H 900  -x -0.5671 -y -0.56698 -w 0.2 "
                "-i "
                "10000 -o mandelbrot.png"
             << endl;
        return EXIT_FAILURE;
    }
  }

  Image img(params.HD_IMG_WIDTH, params.HD_IMG_HEIGHT);
  Stats stats;
  vector<thread> threads;
  for (int mod = 0; mod < threadCount; ++mod) {
    threads.emplace_back(threadWorker<long double>, params, &img, mod);
  }
  for (auto &thread : threads) {
    thread.join();
  }

  for (int iy = 0; iy < params.HD_IMG_HEIGHT; ++iy)
    for (int ix = 0; ix < params.HD_IMG_WIDTH; ++ix) {
      int iters = img.iterations(ix, iy);
      if (iters != params.maxIterationCount) {
        stats(iters);
      }
    }
  stats(params.maxIterationCount);
  stats.preparePercentile();
  for (int iy = 0; iy < params.HD_IMG_HEIGHT; ++iy)
    for (int ix = 0; ix < params.HD_IMG_WIDTH; ++ix) {
      double shade = ix > 0 && iy > 0 && ix < params.HD_IMG_WIDTH - 1 &&
                             iy < params.HD_IMG_HEIGHT - 1
                         ? img.hillshade(ix, iy)
                         : 0;
      setColor(stats, &img, params.maxIterationCount, ix, iy, shade);
    }

  bool ok = img.writePng(params);

  ofstream histogram("histogram.csv");
  histogram << stats;
  histogram.close();

  return ok ? 0 : 1;
}
