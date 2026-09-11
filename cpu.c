#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint8_t V[16];                      // 8 bit registers V0-VF
    uint16_t I;                         // 16 bit index register
    uint16_t stack[64];                 // 64 byte stack for subroutine addresses
    uint8_t s_pointer;                  // 8 bit stack pointer
    uint8_t delay_t;                    // 8 bit delay timer
    uint8_t sound_t;                    // 8 bit sound timer
    uint8_t fbuff[64 * 32];             // 64x32 bit frame buffer
    uint16_t PC;                        // 16 bit program counter
    uint8_t mem[4096];                  // 4096 bytes of addressable memory (free memory starts at 0x200)
    uint8_t keyboard[16];               // array used for representing input

    // FLAGS
    bool vy_shift;                      // if true, uses VY for 8XY6 and 8XYE
    bool vx_jump;                       // if true, uses VX for BNNN
    bool retro_mem_load;                // if true, sets I to I + x + 1 when loading / storing registers
    bool fx1e_overflow;                 // if true, sets V[F] flag to overflow 
} CPU;


// unsigned short index_r = 0;          
// unsigned short stack[64] = {0};      // 64 byte stack
// unsigned char s_pointer = 0;         // 8 bit stack pointer

// unsigned char delay_t = 0;           // 8 bit delay timer
// unsigned char sound_t = 0;           // 8 bit sound timer

// unsigned char fbuff[64 * 32] = {0};  // 64x32 bit frame buffer
// unsigned short PC = 0x200;           // 16 bit program counter
// unsigned char mem[4096] = {0};       // 4096 bytes of addressable memory (free memory starts at 0x200)
// unsigned char keyboard[16] = {0};    // array used for representing input

// Initializer, sets all values aside from PC to 0
void initCPU(CPU *cpu) {
    *cpu = (CPU){0};
    cpu->fx1e_overflow = true;
    cpu->PC = 0x200;
}

void tick(CPU *cpu) {
    // Fetch opcode
    uint8_t opA = cpu->mem[cpu->PC];
    uint8_t opB = cpu->mem[cpu->PC + 1];

    // Decode opcode
    uint8_t x = opA & 0x0F;                    // second nibble
    uint8_t y = (opB >> 4) & 0x0F;             // third nibble
    uint8_t n = opB & 0x0F;                    // fourth nibble
    uint8_t nn = opB;                          // last two nibbles
    uint16_t nnn = ((opA & 0x0F) << 8) | opB;  // last three nibbles

    // Move to next instruction
    cpu->PC += 2;

    switch ((opA >> 4) & 0x0F) {

        case 0x0:
            if (nn == 0xE0) {
                // CLS
                for (int i = 0; i < sizeof(cpu->fbuff); i++) {
                    cpu->fbuff[i] = 0;
                }
            }
            else if (nn == 0xEE) {
                // RET
                cpu->s_pointer -= 1;
                cpu->PC = cpu->stack[cpu->s_pointer];
            }
            break;


        case 0x1:
            // JP addr
            cpu->PC = nnn;
            break;


        case 0x2:
            // CALL addr
            cpu->stack[cpu->s_pointer] = cpu->PC;
            cpu->s_pointer += 1;
            cpu->PC = nnn;
            break;


        case 0x3:
            // SE Vx, byte
            if (cpu->V[x] == nn) {
                cpu->PC += 2;
            }
            break;


        case 0x4:
            // SNE Vx, byte
            if (cpu->V[x] != nn) {
                cpu->PC += 2;
            }
            break;


        case 0x5:
            // SE Vx, Vy
            if (n == 0x0 && cpu->V[x] == cpu->V[y]) {
                cpu->PC += 2;
            }
            break;


        case 0x6:
            // LD Vx, byte
            cpu->V[x] = nn;
            break;


        case 0x7:
            // ADD Vx, byte
            cpu->V[x] += nn;
            break;


        case 0x8:
            switch (n) {

                case 0x0:
                    // LD Vx, Vy
                    cpu->V[x] = cpu->V[y];
                    break;

                case 0x1:
                    // OR Vx, Vy
                    cpu->V[x] |= cpu->V[y];
                    break;

                case 0x2:
                    // AND Vx, Vy
                    cpu->V[x] &= cpu->V[y];
                    break;

                case 0x3:
                    // XOR Vx, Vy
                    cpu->V[x] ^= cpu->V[y];
                    break;

                case 0x4:
                    // ADD Vx, Vy
                    cpu->V[0xF] =
                        (cpu->V[x] + cpu->V[y]) > 0xFF;

                    cpu->V[x] += cpu->V[y];
                    break;

                case 0x5:
                    // SUB Vx, Vy
                    cpu->V[0xF] = cpu->V[x] >= cpu->V[y];
                    cpu->V[x] -= cpu->V[y];
                    break;

                case 0x6:
                    // SHR Vx {, Vy}
                    if (cpu->vy_shift) {
                        cpu->V[x] = cpu->V[y];
                    }

                    cpu->V[0xF] = cpu->V[x] & 0x01;
                    cpu->V[x] >>= 1;
                    break;

                case 0x7:
                    // SUBN Vx, Vy
                    cpu->V[0xF] = cpu->V[y] >= cpu->V[x];
                    cpu->V[x] = cpu->V[y] - cpu->V[x];
                    break;

                case 0xE:
                    // SHL Vx {, Vy}
                    if (cpu->vy_shift) {
                        cpu->V[x] = cpu->V[y];
                    }

                    cpu->V[0xF] = (cpu->V[x] >> 7) & 0x01;
                    cpu->V[x] <<= 1;
                    break;

                default:
                    break;
            }
            break;


        case 0x9:
            // SNE Vx, Vy
            if (n == 0x0 && cpu->V[x] != cpu->V[y]) {
                cpu->PC += 2;
            }
            break;


        case 0xA:
            // LD I, addr
            cpu->I = nnn;
            break;


        case 0xB:
            // JP V0, addr
            if (cpu->vx_jump) {
                cpu->PC = nnn + cpu->V[x];
            }
            else {
                cpu->PC = nnn + cpu->V[0];
            }
            break;


        case 0xC:
            // RND Vx, byte
            cpu->V[x] = rand() & nn;
            break;


        case 0xD: {
            // DRW Vx, Vy, nibble
            cpu->V[0xF] = 0;

            for (int row = 0; row < n; row++) {

                uint8_t sprite = cpu->mem[cpu->I + row];

                for (int col = 0; col < 8; col++) {

                    uint8_t bit = (sprite >> (7 - col)) & 1;

                    if (bit == 0) {
                        continue;
                    }

                    int px = (cpu->V[x] + col) % 64;
                    int py = (cpu->V[y] + row) % 32;

                    int index = py * 64 + px;

                    if (cpu->fbuff[index] == 1) {
                        cpu->V[0xF] = 1;
                    }

                    cpu->fbuff[index] ^= 1;
                }
            }

            break;
        }


        case 0xE:
            if (nn == 0x9E) {
                // SKP Vx
                if (cpu->keyboard[cpu->V[x]] == 1) {
                    cpu->PC += 2;
                }
            }
            else if (nn == 0xA1) {
                // SKNP Vx
                if (cpu->keyboard[cpu->V[x]] == 0) {
                    cpu->PC += 2;
                }
            }
            break;


        case 0xF:
            switch (nn) {

                case 0x07:
                    // LD Vx, DT
                    cpu->V[x] = cpu->delay_t;
                    break;


                case 0x0A:
                    // LD Vx, K
                    cpu->PC -= 2;

                    for (int i = 0; i < sizeof(cpu->keyboard); i++) {
                        if (cpu->keyboard[i] == 1) {
                            cpu->V[x] = i;
                            cpu->PC += 2;
                            break;
                        }
                    }
                    break;


                case 0x15:
                    // LD DT, Vx
                    cpu->delay_t = cpu->V[x];
                    break;


                case 0x18:
                    // LD ST, Vx
                    cpu->sound_t = cpu->V[x];
                    break;


                case 0x1E:
                    // ADD I, Vx
                    if (cpu->fx1e_overflow) {
                        cpu->V[0xF] =
                            (cpu->I + cpu->V[x]) >= 0x1000;
                    }

                    cpu->I += cpu->V[x];
                    break;


                case 0x29:
                    // LD F, Vx
                    cpu->I = cpu->V[x] * 5;
                    break;


                case 0x33:
                    // LD B, Vx
                    cpu->mem[cpu->I] =
                        cpu->V[x] / 100;

                    cpu->mem[cpu->I + 1] =
                        (cpu->V[x] / 10) % 10;

                    cpu->mem[cpu->I + 2] =
                        cpu->V[x] % 10;
                    break;


                case 0x55:
                    // LD [I], Vx
                    for (int i = 0; i <= x; i++) {
                        cpu->mem[cpu->I + i] = cpu->V[i];
                    }

                    if (cpu->retro_mem_load) {
                        cpu->I += x + 1;
                    }
                    break;


                case 0x65:
                    // LD Vx, [I]
                    for (int i = 0; i <= x; i++) {
                        cpu->V[i] = cpu->mem[cpu->I + i];
                    }

                    if (cpu->retro_mem_load) {
                        cpu->I += x + 1;
                    }
                    break;


                default:
                    break;
            }
            break;


        default:
            break;
    }
}



