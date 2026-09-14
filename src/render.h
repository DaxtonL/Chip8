#ifndef RENDER_H
#define RENDER_H

#include "cpu.h"

#define SCREEN_SCALE 10

int render_init(void);
void render_handle_events(CPU *cpu, int *running);
void render_screen(CPU *cpu);
void render_cleanup(void);

#endif