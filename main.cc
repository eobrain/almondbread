#include <stdlib.h>
#include <complex>
#include <iostream>
#include <math.h>
#include <array>
#include <SDL2/SDL.h>

constexpr int WINDOW_WIDTH = 1000;
constexpr float centerRe = -0.568;
constexpr float centerIm = -0.567;
constexpr float width = 0.004;
constexpr int maxIterationCount = 10000;
const int logMax = log(maxIterationCount);

constexpr int MANY = -1;

using std::array;
using std::complex;
using std::cout;
using std::endl;
using std::flush;

namespace
{
    array<int, WINDOW_WIDTH * WINDOW_WIDTH> dataBuf;

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
                return i;
            }
        }
        return MANY;
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
        if (iters == MANY)
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            return;
        }

        Uint8 red = clamp(256 * log(iters) / logMax);
        Uint8 green = red;
        Uint8 blue = red;
        //Uint8 blue = 256 - red;
        //cout << "red=" << (int)red << " blue=" << (int)blue << endl;
        SDL_SetRenderDrawColor(renderer, red, green, blue, SDL_ALPHA_OPAQUE);
    }

} //namespace

int main(void)
{
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;

    for (int ix = 0; ix < WINDOW_WIDTH; ++ix)
    {
        for (int iy = 0; iy < WINDOW_WIDTH; ++iy)
        {
            data(ix, iy) = iterations(ix, iy);
        }
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
        cout << "\r" << 100 * ix / WINDOW_WIDTH << "%" << flush;
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
    while (1)
    {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
    }
    cout << endl;
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
