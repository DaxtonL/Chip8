#include <stdio.h>
#include <SDL2/SDL.h>
#include "cpu.c"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <rom>\n", argv[0]);
        return 1;
    }

    CPU cpu;
    initCPU(&cpu);

    if (!chip8_load_rom(&cpu, argv[1]))
    {
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Test",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640,
        320,
        SDL_WINDOW_SHOWN
    );

    if (window == NULL)
    {
        printf("Window failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if (renderer == NULL)
    {
        printf("Renderer failed: %s\n", SDL_GetError());
        return 1;
    }

    // Set drawing color to white
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    int running = 1;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = 0;
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Set color for square
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        // Draw square
        for (int x = 0; x < 64; x++) {
            for (int y = 0; y < 32; y++) {
                if (cpu.fbuff[(y * 64) + x]) {
                    SDL_Rect square = {
                        x * 10,
                        y * 10,
                        10,
                        10
                    };
                    SDL_RenderFillRect(renderer, &square);
                }
            }
        }

        // Display frame
        SDL_RenderPresent(renderer);

        // CPU cycle
        tick(&cpu);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}