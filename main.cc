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

    constexpr int WIDTH = 1600;
    constexpr int HEIGHT = 900;
    constexpr float centerRe = -0.5671;
    constexpr float centerIm = -0.56698;
    constexpr float width = 0.02;
    constexpr int maxIterationCount = 1000000;

    array<int, WIDTH * HEIGHT> dataBuf;
    array<int, WIDTH * HEIGHT> smoothedBuf;
    int maxFinite = -1;

    inline int &data(int ix, int iy)
    {
        // Optimized for iy changing faster
        return dataBuf[HEIGHT * ix + iy];
    }

    inline int &smoothed(int ix, int iy)
    {
        return smoothedBuf[HEIGHT * ix + iy];
    }

    constexpr int CENTERWEIGHT = 4;

    inline void smooth()
    {
        for (int ix = 1; ix < WIDTH - 1; ++ix)
            for (int iy = 1; iy < HEIGHT - 1; ++iy)
                smoothed(ix, iy) = (CENTERWEIGHT * data(ix, iy) + data(ix + 1, iy) + data(ix - 1, iy) + data(ix, iy + 1) + data(ix, iy - 1)) / (CENTERWEIGHT + 4);
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

    void setColor(SDL_Renderer *renderer, int ix, int iy)
    {
        //std::cout << "c=" << c << std::endl;
        int iters = smoothed(ix, iy);
        if (iters == maxIterationCount)
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            return;
        }

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
    smooth();

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    for (int ix = 1; ix < WIDTH - 1; ++ix)
    {
        for (int iy = 1; iy < HEIGHT - 1; ++iy)
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
         << " log=" << log(maxFinite) << endl;
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
