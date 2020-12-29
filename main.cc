#include <stdlib.h>
#include <complex>
#include <iostream>
#include <math.h>
#include <array>
#include <vector>
#include <thread>

#include "lodepng.h"

using std::array;
using std::complex;
using std::cout;
using std::endl;
using std::flush;
using std::thread;
using std::vector;

typedef unsigned char Uint8;

namespace
{

    constexpr int WIDTH = 160 + 2;
    constexpr int HEIGHT = 90 + 2;
    constexpr float centerRe = -0.5671;
    constexpr float centerIm = -0.56698;
    constexpr float width = 0.2;
    constexpr int maxIterationCount = 100;

    array<int, WIDTH * HEIGHT> dataBuf;
    array<int, WIDTH * HEIGHT> smoothedBuf;
    array<Uint8, WIDTH * HEIGHT * 4> imageBuf;
    int maxFinite = -1;

    inline int &data(int ix, int iy)
    {
        // Optimized for iy changing faster
        return dataBuf[HEIGHT * ix + iy];
    }

    inline int &smoothed(int ix, int iy)
    {
        return smoothedBuf[HEIGHT * (ix - 1) + (iy - 1)];
    }

    inline Uint8 &image(int ix, int iy, int layer)
    {
        return imageBuf[4 * HEIGHT * (ix - 1) + 4 * (iy - 1) + layer];
    }
    constexpr int CENTERWEIGHT = 4;

    inline void smooth()
    {
        for (int ix = 1; ix < WIDTH - 1; ++ix)
            for (int iy = 1; iy < HEIGHT - 1; ++iy)
                smoothed(ix, iy) = data(ix, iy);
        //smoothed(ix, iy) = (CENTERWEIGHT * data(ix, iy) + data(ix + 1, iy) + data(ix - 1, iy) + data(ix, iy + 1) + data(ix, iy - 1)) / (CENTERWEIGHT + 4);
    }

    constexpr Uint8 clamp(int color)
    {
        return color >= 255 ? 255 : color;
    }
    int iterations(complex<float> c)
    {
        complex<float> z = 0;
        for (int i = 0; i < maxIterationCount; ++i)
        {
            z = z * z + c;
            //std::cout << i << ": " << z << std::endl;
            if (std::abs(z) > 2)
            {
                if (i > maxFinite)
                {
                    maxFinite = i;
                }
                return i;
            }
        }
        return maxIterationCount;
    }

    int iterations(int ix, int iy)
    {
        float scale = width / WIDTH;
        float cRe = scale * (ix - WIDTH / 2) + centerRe;
        float cIm = scale * (iy - HEIGHT / 2) + centerIm;
        complex<float> c = {cRe, cIm};
        return iterations(c);
    }

    const int COLOR_SCALE = 32;

    void setColor(int ix, int iy)
    {
        //std::cout << "c=" << c << std::endl;
        int iters = smoothed(ix, iy);
        if (iters == maxIterationCount)
        {
            image(ix, iy, 0) = 0;
            image(ix, iy, 1) = 0;
            image(ix, iy, 2) = 0;
            image(ix, iy, 3) = 255;
            return;
        }

        // RGBA
        image(ix, iy, 0) = clamp(COLOR_SCALE * log(iters));
        image(ix, iy, 1) = clamp(COLOR_SCALE * sqrt(iters));
        image(ix, iy, 2) = clamp(COLOR_SCALE * iters);
        image(ix, iy, 3) = 255;
    }

    int threadCount = thread::hardware_concurrency();

    void threadWorker(int mod)
    {
        for (int ix = mod; ix < WIDTH; ix += threadCount)
        {
            for (int iy = 0; iy < HEIGHT; ++iy)
            {
                data(ix, iy) = iterations(ix, iy);
            }
        }
        cout << "Finished thread " << mod << endl;
    }
} //namespace

int main(void)
{
    cout << "threadCount=" << threadCount << endl;

    vector<thread> threads;
    for (int mod = 0; mod < threadCount; ++mod)
    {
        threads.emplace_back(threadWorker, mod);
    }
    for (auto &thread : threads)
    {
        thread.join();
    }
    smooth();

    for (int ix = 1; ix < WIDTH - 1; ++ix)
        for (int iy = 1; iy < HEIGHT - 1; ++iy)
            setColor(ix, iy);

    cout << "maxFinite=" << maxFinite
         << " sqrt=" << sqrt(maxFinite)
         << " log=" << log(maxFinite) << endl
         << int(COLOR_SCALE * maxFinite) << " "
         << int(COLOR_SCALE * sqrt(maxFinite)) << " "
         << int(COLOR_SCALE * log(maxFinite)) << endl;
    unsigned error = lodepng::encode("mandelbrot.png", imageBuf.data(), WIDTH - 2, HEIGHT - 2);

    //if there's an error, display it
    if (error)
    {
        cout << "encoder error " << error << ": " << lodepng_error_text(error) << endl;
        //cout << "encoder error " << error << ": " << error << endl;
        return error;
    }

    return EXIT_SUCCESS;
}
