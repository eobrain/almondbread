#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include <array>
#include <complex>
#include <iostream>
#include <thread>
#include <vector>

#include "lodepng.h"

using std::cerr;
using std::complex;
using std::cout;
using std::endl;
using std::flush;
using std::ostream;
using std::string;
using std::thread;
using std::vector;

namespace {

// constexpr int imgWidth = 1400;
// constexpr int imgHeight = 900;
// constexpr double centerRe = -0.5671;
// constexpr double centerIm = -0.56698;
// constexpr double width = 0.2;
// constexpr int maxIterationCount = 10000;

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
      << " imgHeight=" << p.imgHeight << " centerRe=" << p.centerRe
      << " centerIm=" << p.centerIm << " width=" << p.width
      << " maxIterationCount=" << p.maxIterationCount;
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

const int COLOR_SCALE = 8;

void setColor(Image *img, int maxIterationCount, int ix, int iy) {
  // std::cout << "c=" << c << std::endl;
  int iters = img->iterations(ix, iy);
  if (iters == maxIterationCount) {
    img->pixel(ix, iy, 0) = 0;
    img->pixel(ix, iy, 1) = 0;
    img->pixel(ix, iy, 2) = 0;
    img->pixel(ix, iy, 3) = 255;
    return;
  }

  // RGBA
  img->pixel(ix, iy, 0) = clamp(COLOR_SCALE * log(iters));
  img->pixel(ix, iy, 1) = clamp(COLOR_SCALE * sqrt(iters));
  img->pixel(ix, iy, 2) = clamp(COLOR_SCALE * iters);
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

  return ok ? 0 : 1;
}
