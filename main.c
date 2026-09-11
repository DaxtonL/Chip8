#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

unsigned char V[16] = {0};           // 8 bit registers V0-VF
unsigned short index_r = 0;          // 16 bit index register
unsigned short stack[64] = {0};      // 64 byte stack
unsigned char s_pointer = 0;         // 8 bit stack pointer
unsigned char delay_t = 0;           // 8 bit delay timer
unsigned char sound_t = 0;           // 8 bit sound timer
unsigned char fbuff[64 * 32] = {0};  // 64x32 bit frame buffer
unsigned short PC = 0x200;           // 16 bit program counter
unsigned char mem[4096] = {0};       // 4096 bytes of addressable memory (free memory starts at 0x200)
unsigned char keyboard[16] = {0};    // array used for representing input

// FLAGS
bool vy_shift = false;          // if true, uses VY for 8XY6 and 8XYE
bool vx_jump = false;           // if true, uses VX for BNNN
bool retro_mem_load = false;    // if true, sets I to I + x + 1 when loading / storing registers

void tick() {
    unsigned char op = (mem[PC] >> 4) & 0b1111;      // first nibble of opcode
    unsigned char x = mem[PC] & 0b1111;              // second nibble of opcode
    unsigned char y = (mem[PC + 1] >> 4) & 0b1111;   // third nibble of opcode
    unsigned char n = mem[PC + 1] & 0b1111;          // fourth nibble of opcode
    unsigned char nn = mem[PC + 1];                  // second byte
    unsigned short nnn = (x << 4) | nn;              // 2nd, 3rd, and 4th nibble
    PC += 2;
    switch (op) {
        case 0x0:
            if (nn == 0xE0) {
                // CLS
                for (int i = 0; i < sizeof(fbuff); i++) {
                    fbuff[i] = 0x0;
                }
            } else if (nn == 0xEE) {
                // RET
                s_pointer -= 1;
                PC = stack[s_pointer];
            }
            break;
        case 0x1:
            // JP addr
            PC = nnn;
            break;
        case 0x2:
            // CALL addr
            stack[s_pointer] = PC;
            s_pointer += 1;
            PC = nnn;
            break;
        case 0x3:
            // SE Vx, byte
            // skip if vx = nn
            if (V[x] == nn) {
                PC += 2;
            }
            break;
        case 0x4:
            // SNE Vx, byte
            // skip if vx != nn
            if (V[x] != nn) {
                PC += 2;
            }
            break;
        case 0x5:
            // SE Vx, Vy
            // skip if vx = vy
            if (V[x] == V[y]) {
                PC += 2;
            }
            break;
        case 0x6:
            // LD Vx, byte
            // set vx to nn
            V[x] = nn;
            break;
        case 0x7:
            // ADD Vx, byte
            V[x] = V[x] + nn;
            break;
        case 0x8:
            switch (n) {
                case 0x0:
                    // LD Vx, Vy
                    V[x] = V[y];
                    break;
                case 0x1:
                    // OR Vx, Vy
                    V[x] = V[x] | V[y];
                    break;
                case 0x2:
                    // AND Vx, Vy
                    V[x] = V[x] & V[y];
                    break;
                case 0x3:
                    // XOR Vx, Vy
                    V[x] = V[x] ^ V[y];
                    break;
                case 0x4:
                    // ADD Vx, Vy
                    if ((V[x] + V[y]) > 0xFF) {
                        // Set overflow flag
                        V[0xF] = (V[x] + V[y] > 0xFF);
                    }
                    V[x] = (char) ((V[x] + V[y]) & 0xFF);
                    break;
                case 0x5:
                    // SUB Vx, Vy
                    if (V[x] < V[y]) {
                        V[0xF] = (V[x] + V[y] > 0xFF);
                    }
                    V[x] = V[x] - V[y];
                    break;
                case 0x6:
                    // SHR Vx {, Vy}
                    if (vy_shift) {
                        V[x] = V[y];
                    }
                    V[0xF] = 0x01 & V[x];
                    V[x] = V[x] >> 1;
                    break;
                case 0x7:
                    // SUBN Vx, Vy
                    if (V[y] < V[x]) {
                        V[0xF] = (V[x] + V[y] > 0xFF);
                    }
                    V[x] = V[y] - V[x];
                    break;
                case 0xE:
                    // SHL Vx {, Vy}
                    if (vy_shift) {
                        V[x] = V[y];
                    }
                    V[0xF] = (V[x] >> 7) & 1;
                    V[x] = V[x] << 1;
                    break;
                default:
                    break;
            }
            break;
        case 0x9:
            // SNE Vx, Vy
            if (V[x] != V[y]) {
                PC += 2;
            }
            break;
        case 0xA:
            // LD I, addr
            index_r = nnn;
            break;
        case 0xB:
            // JP V0, addr
            if (vx_jump) {
                PC = nnn + V[n];
            } else {
                PC = nnn + V[0];
            }
            break;
        case 0xC:
            // RND Vx, byte
            V[x] = (unsigned char)rand() & nn;
            break;
        case 0xD:
            // TODO
            int posx = V[x] % 64;
            int posy = V[y] % 32;
            V[0xF] = 0x0;
            for (int i = 0; i < n; i++) {

            }
            break;
        case 0xE:
            if (nn == 0x9E) {
                //SKP Vx
                if (keyboard[V[x]] == 0x1) {
                    PC += 2;
                }
            } else if (nn == 0xA1) {
                //SKNP Vx
                if (keyboard[V[x]] == 0x0) {
                    PC += 2;
                }
            }
            break;
        case 0xF:
            switch (nn) {
                case 0x07:
                    //LD Vx, DT
                    V[x] = delay_t;
                     break;
                case 0x0A:
                    // LD Vx, K
                    PC -= 2;
                    for (int i = 0; i < sizeof(keyboard); i++) {
                        if (keyboard[i] == 0x1) {
                            V[x] = (char) i;
                            PC += 2;
                            break;
                        }
                    }
                    break;
                case 0x15:
                    // LD DT, Vx
                    delay_t = V[x];
                    break;
                case 0x18:
                    // LD ST, Vx
                    sound_t = V[x];
                    break;
                case 0x1e:
                    // Add I, Vx
                    if ((index_r + V[x]) >= 0x1000) {
                        V[0xF] = 0x1;
                    }
                    index_r += V[x];
                    break;
                case 0x29:
                    // LD F, Vx
                    index_r = V[x] * 5;
                    break;
                case 0x33:
                    // LD B, Vx
                    mem[index_r] = V[x] / 100;
                    mem[index_r + 1] = (V[x] / 10) % 10;
                    mem[index_r + 2] = V[x] % 10;
                    break;
                case 0x55:
                    // LD [I], Vx
                    for (int i = 0; i <= x; i++) {
                        mem[index_r + i] = V[i]; 
                    }
                    if (retro_mem_load) {
                        index_r = index_r + x + 1;
                    }
                    break;
                case 0x65:
                    // LD Vx, [I]
                    for (int i = 0; i <= x; i++) {
                        V[i] = mem[index_r + i]; 
                    }
                    if (retro_mem_load) {
                        index_r = index_r + x + 1;
                    }
                    break;
                defualt:
                    break;
            }
            break;
        default:
            break;
    }
}

int main() {
    tick();
    return 0;
}



