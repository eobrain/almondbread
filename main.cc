#include <stdlib.h>
#include <complex>
#include <iostream>

#include <SDL2/SDL.h>

constexpr int WINDOW_WIDTH = 1000;
constexpr float centerRe = -0.6;
constexpr float centerIm = 0.5;
constexpr float width = 0.3;
constexpr int maxIterationCount = 1000;

namespace
{
    void setColor(SDL_Renderer *renderer, int ix, int iy)
    {
        using namespace std::complex_literals;
        float cRe = width * (ix - WINDOW_WIDTH / 2) / WINDOW_WIDTH + centerRe;
        float cIm = width * (iy - WINDOW_WIDTH / 2) / WINDOW_WIDTH + centerIm;
        std::complex<float> c = {cRe, cIm};
        std::complex<float> z = 0;
        //std::cout << "c=" << c << std::endl;

        for (int i = 0; i < maxIterationCount; ++i)
        {
            z = z * z + c;
            //std::cout << i << ": " << z << std::endl;
            if (std::abs(z) > 2)
            {
                Uint8 red = i & 0xff;
                Uint8 blue = 256 - red;
                SDL_SetRenderDrawColor(renderer, red, 128, blue, SDL_ALPHA_OPAQUE);
                return;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    }

} //namespace

int main(void)
{
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_WIDTH, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    for (int ix = 0; ix < WINDOW_WIDTH; ++ix)
    {
        for (int iy = 0; iy < WINDOW_WIDTH; ++iy)
        {
            setColor(renderer, ix, iy);
            SDL_RenderDrawPoint(renderer, ix, iy);
        }
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
        std::cout << "\r" << 100 * ix / WINDOW_WIDTH << "%";
        std::cout.flush();
    }
    SDL_RenderPresent(renderer);
    while (1)
    {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
    }
    std::cout << std::endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
