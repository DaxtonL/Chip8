#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "render.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static int get_chip8_key(SDL_Keycode key)
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

int render_init(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 0;
    }

    window = SDL_CreateWindow(
        "CHIP-8 Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        64 * SCREEN_SCALE,
        32 * SCREEN_SCALE,
        SDL_WINDOW_SHOWN
    );

    if (window == NULL)
    {
        printf("Window failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if (renderer == NULL)
    {
        printf("Renderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    return 1;
}

void render_handle_events(CPU *cpu, int *running)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            *running = 0;
        }

        if (event.type == SDL_KEYDOWN)
        {
            int key = get_chip8_key(event.key.keysym.sym);

            if (key != -1)
                cpu->keypad[key] = true;
        }

        if (event.type == SDL_KEYUP)
        {
            int key = get_chip8_key(event.key.keysym.sym);

            if (key != -1)
                cpu->keypad[key] = false;
        }
    }
}

void render_screen(CPU *cpu)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            if (cpu->fbuff[y * 64 + x])
            {
                SDL_Rect pixel = {
                    x * SCREEN_SCALE,
                    y * SCREEN_SCALE,
                    SCREEN_SCALE,
                    SCREEN_SCALE
                };

                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

void render_cleanup(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}