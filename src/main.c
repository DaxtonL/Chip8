#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "cpu.h"
#include "render.h"

#define CPU_HZ 600
#define TIMER_HZ 60
#define FRAME_HZ 60

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

    if (!render_init())
    {
        return 1;
    }

    int running = 1;

    Uint64 frequency = SDL_GetPerformanceFrequency();
    Uint64 last_time = SDL_GetPerformanceCounter();

    double cpu_accumulator = 0.0;
    double timer_accumulator = 0.0;
    double frame_accumulator = 0.0;

    const double cpu_period = 1.0 / CPU_HZ;
    const double timer_period = 1.0 / TIMER_HZ;
    const double frame_period = 1.0 / FRAME_HZ;

    while (running)
    {
        Uint64 current_time = SDL_GetPerformanceCounter();

        double elapsed =
            (double)(current_time - last_time) / frequency;

        last_time = current_time;

        // Prevent huge jumps if the program is paused/debugged
        if (elapsed > 0.1)
            elapsed = 0.1;

        cpu_accumulator += elapsed;
        timer_accumulator += elapsed;
        frame_accumulator += elapsed;

        // Handle keyboard/window events
        render_handle_events(&cpu, &running);

        // Run CPU at 600 Hz
        while (cpu_accumulator >= cpu_period)
        {
            tick(&cpu);
            cpu_accumulator -= cpu_period;
        }

        // Run CHIP-8 timers at 60 Hz
        while (timer_accumulator >= timer_period)
        {
            if (cpu.delay_t > 0)
                cpu.delay_t--;

            if (cpu.sound_t > 0)
                cpu.sound_t--;

            timer_accumulator -= timer_period;
        }

        // Render at 60 FPS
        if (frame_accumulator >= frame_period)
        {
            render_screen(&cpu);
            frame_accumulator -= frame_period;
        }

        // Give the OS some CPU time
        SDL_Delay(1);
    }

    render_cleanup();

    return 0;
}