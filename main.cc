#include <stdlib.h>
#include <complex>
#include <iostream>
#include <math.h>
#include <array>
#include <vector>
#include <thread>
#include <SDL2/SDL.h>

using std::array;
using std::complex;
using std::cout;
using std::endl;
using std::flush;
using std::thread;
using std::vector;

namespace
{

    constexpr int WINDOW_WIDTH = 1000;
    constexpr float centerRe = -0.568;
    constexpr float centerIm = -0.567;
    constexpr float width = 1;
    constexpr int maxIterationCount = 5000;

    array<int, WINDOW_WIDTH * WINDOW_WIDTH> dataBuf;
    int maxFinite = -1;

    inline int &data(int ix, int iy)
    {
        return dataBuf[ix + WINDOW_WIDTH * iy];
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
        float cRe = width * (ix - WINDOW_WIDTH / 2) / WINDOW_WIDTH + centerRe;
        float cIm = width * (iy - WINDOW_WIDTH / 2) / WINDOW_WIDTH + centerIm;
        complex<float> c = {cRe, cIm};
        return iterations(c);
    }

    void setColor(SDL_Renderer *renderer, int ix, int iy)
    {
        //std::cout << "c=" << c << std::endl;
        int iters = data(ix, iy);
        if (iters == maxIterationCount)
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            return;
        }
        //int dx = iters - data(ix - 1, iy);
        //int dy = iters - data(ix, iy - 1);

        //int hue = 128 - 256 * iters / maxFinite;
        //ColorSpace::Lab lab(100 * log(iters) / log(maxFinite), 100*log(iters)/log(maxFinite)-100, 0);
        //ColorSpace::Rgb rgb;
        //lab.To<ColorSpace::Rgb>(&rgb);

        Uint8 b = clamp(10 * log(iters));
        Uint8 g = clamp(10 * sqrt(iters));
        ;
        Uint8 r = clamp(10 * iters);
        //cout << "red=" << (int)red << " blue=" << (int)blue << endl;
        SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
    }

    int threadCount = thread::hardware_concurrency();

    void threadWorker(int mod)
    {
        for (int ix = mod; ix < WINDOW_WIDTH; ix += threadCount)
        {
            for (int iy = 0; iy < WINDOW_WIDTH; ++iy)
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
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;

    vector<thread> threads;
    for (int mod = 0; mod < threadCount; ++mod)
    {
        threads.emplace_back(threadWorker, mod);
    }
    for (auto &thread : threads)
    {
        thread.join();
    }

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_WIDTH, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    for (int ix = 1; ix < WINDOW_WIDTH; ++ix)
    {
        for (int iy = 1; iy < WINDOW_WIDTH; ++iy)
        {
            setColor(renderer, ix, iy);
            SDL_RenderDrawPoint(renderer, ix, iy);
        }
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
        //cout << "\r" << 100 * ix / WINDOW_WIDTH << "%" << flush;
    }
    SDL_RenderPresent(renderer);
    cout << "maxFinite=" << maxFinite
         << " sqrt=" << sqrt(maxFinite)
         << "log=" << log(maxFinite) << endl;
    while (1)
    {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
    }
    if (SDL_SaveBMP(SDL_GetWindowSurface(window), "mandelbrot.bmp") != 0)
    {
        // Error saving bitmap
        printf("SDL_SaveBMP failed: %s\n", SDL_GetError());
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
