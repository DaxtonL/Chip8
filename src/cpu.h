#ifndef CPU_H
#define CPU_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
uint8_t V[16];                      // 8 bit registers V0-VF
uint16_t I;                         // 16 bit index register
uint16_t stack[64];                 // Stack for subroutine addresses
uint8_t s_pointer;                  // Stack pointer
uint8_t delay_t;                    // Delay timer
uint8_t sound_t;                    // Sound timer
uint8_t fbuff[64 * 32];             // 64x32 frame buffer
uint16_t PC;                        // Program counter
uint8_t mem[4096];                  // 4096 bytes of addressable memory
uint8_t keypad[16];                 // CHIP-8 keypad

// FLAGS
bool vy_shift;                      // VY for 8XY6 and 8XYE
bool vx_jump;                       // VX for BNNN
bool retro_mem_load;                // I += x + 1 for FX55/FX65
bool fx1e_overflow;                 // VF set on FX1E overflow

} CPU;

void initCPU(CPU *cpu);
void tick(CPU *cpu);
bool chip8_load_rom(CPU *chip8, const char *filename);

#endif
