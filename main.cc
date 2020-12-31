#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include <array>
#include <complex>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <thread>
#include <vector>

#include "lodepng.h"

using std::array;
using std::cerr;
using std::complex;
using std::cout;
using std::endl;
using std::flush;
using std::map;
using std::ofstream;
using std::ostream;
using std::string;
using std::thread;
using std::vector;

namespace {

constexpr int INT_MIN = std::numeric_limits<int>::min();
constexpr int INT_MAX = std::numeric_limits<int>::max();

struct Params {
  int imgWidth = 1400;
  int imgHeight = 900;
  double centerRe = -0.5671;
  double centerIm = -0.56698;
  double width = 0.2;
  int maxIterationCount = 10000;
  const char *outputFileName = "mandelbrot.png";
};
ostream &operator<<(ostream &out, const Params &p) {
  out << "imgWidth=" << p.imgWidth << " imgHeight=" << p.imgHeight
      << " centerRe=" << p.centerRe << " centerIm=" << p.centerIm
      << " width=" << p.width << " maxIterationCount=" << p.maxIterationCount
      << " outputFileName" << p.outputFileName;
  return out;
}

class Stats {
  map<int, int> histogram;
  int min = INT_MAX;
  int max = INT_MIN;

 public:
  void operator()(int i) {
    if (i > max) max = i;
    if (i < min) min = i;
    auto lookup = histogram.find(i);
    int prev = lookup == histogram.end() ? 0 : lookup->second;
    histogram[i] = prev + 1;
  }
  friend ostream &operator<<(ostream &, const Stats &);
};
ostream &operator<<(ostream &out, const Stats &s) {
  out << "Iterations,Count\n";
  for (int i = s.min; i <= s.max; ++i) {
    auto lookup = s.histogram.find(i);
    if (lookup != s.histogram.end()) {
      out << i << "," << lookup->second << "\n";
    }
  }
  return out;
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

  bool writePng(const Params &params) const {
    unsigned error = lodepng::encode(params.outputFileName, _pixels,
                                     params.imgWidth, params.imgHeight);
    if (error) {
      cout << "encoder error " << error << ": " << lodepng_error_text(error)
           << endl;
      return false;
    }
    return true;
  }
};

array<double, 3> hsv2rgb(double h, double s, double v) {
  double c = 0.0, m = 0.0, x = 0.0;
  c = v * s;
  x = c * (1.0 - fabs(fmod(h / 60.0, 2) - 1.0));
  m = v - c;
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

int iterations(int maxIterationCount, complex<double> c) {
  complex<double> z = 0;
  for (int i = 0; i < maxIterationCount; ++i) {
    z = z * z + c;
    if (std::abs(z) > 2) {
      return i;
    }
  }
  return maxIterationCount;
}

int iterations(const Params &params, int ix, int iy) {
  double scale = params.width / params.imgWidth;
  double cRe = scale * (ix - params.imgWidth / 2) + params.centerRe;
  double cIm = scale * (iy - params.imgHeight / 2) + params.centerIm;
  complex<double> c = {cRe, cIm};
  return iterations(params.maxIterationCount, c);
}

const int COLOR_SCALE = 10;

Stats stats;

void setColor(Image *img, int maxIterationCount, int ix, int iy) {
  // std::cout << "c=" << c << std::endl;
  int iters = img->iterations(ix, iy);
  stats(iters);
  if (iters == maxIterationCount) {
    img->pixel(ix, iy, 0) = 0;
    img->pixel(ix, iy, 1) = 0;
    img->pixel(ix, iy, 2) = 0;
    img->pixel(ix, iy, 3) = 255;
    return;
  }

  double value = log(iters) / log(maxIterationCount);
  auto rgb = hsv2rgb(250*value, 0.5, value);
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
  cout << "threadCount=" << threadCount << endl;
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
        params.centerRe = atof(optarg);
        break;
      case 'y':
        params.centerIm = atof(optarg);
        break;
      case 'w':
        params.width = atof(optarg);
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

  cout << params << endl;

  Image img(params.imgWidth, params.imgHeight);

  vector<thread> threads;
  for (int mod = 0; mod < threadCount; ++mod) {
    threads.emplace_back(threadWorker, params, &img, mod);
  }
  for (auto &thread : threads) {
    thread.join();
  }

  for (int iy = 0; iy < params.imgHeight; ++iy)
    for (int ix = 0; ix < params.imgWidth; ++ix) {
      setColor(&img, params.maxIterationCount, ix, iy);
    }

  cout << "maxIterationCount=" << params.maxIterationCount
       << " sqrt=" << sqrt(params.maxIterationCount)
       << " log=" << log(params.maxIterationCount) << endl
       << int(COLOR_SCALE * params.maxIterationCount) << " "
       << int(COLOR_SCALE * sqrt(params.maxIterationCount)) << " "
       << int(COLOR_SCALE * log(params.maxIterationCount)) << endl
       << "centerIterations=" << img.centerIterations() << endl;
  bool ok = img.writePng(params);

  ofstream histogram("histogram.csv");
  histogram << stats;
  histogram.close();

  return ok ? 0 : 1;
}
