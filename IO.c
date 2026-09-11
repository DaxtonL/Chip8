#include <stdio.h>
#include <SDL2/SDL.h>
#include "cpu.c"

int get_chip8_key(SDL_Keycode key)
{
    switch (key)
    {
        case SDLK_1: return 0x1;
        case SDLK_2: return 0x2;
        case SDLK_3: return 0x3;
        case SDLK_4: return 0xC;

        case SDLK_q: return 0x4;
        case SDLK_w: return 0x5;
        case SDLK_e: return 0x6;
        case SDLK_r: return 0xD;

        case SDLK_a: return 0x7;
        case SDLK_s: return 0x8;
        case SDLK_d: return 0x9;
        case SDLK_f: return 0xE;

        case SDLK_z: return 0xA;
        case SDLK_x: return 0x0;
        case SDLK_c: return 0xB;
        case SDLK_v: return 0xF;

        default: return -1;
    }
}

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
            
            if (event.type == SDL_KEYDOWN)
            {
                int key = get_chip8_key(event.key.keysym.sym);

                if (key != -1)
                    cpu.keypad[key] = true;
            }

            if (event.type == SDL_KEYUP)
            {
                int key = get_chip8_key(event.key.keysym.sym);

                if (key != -1)
                    cpu.keypad[key] = false;
            }
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

    SDL_Delay(1);

    return 0;
}

