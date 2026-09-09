#include <stdio.h>
#include <stdbool.h>

char V[8] = {0};            // 8 bit registers V0-VF
short index_r = 0;          // 16 bit index register
char stack[64] = {0};       // 64 byte stack
char s_pointer = 0;           // 8 bit stack pointer
char delay_t = 0;           // 8 bit delay timer
char sound_t = 0;           // 8 bit sound timer
char fbuff[256] = {0};      // 64x32 bit frame buffer, stored as 256 bytes
short PC = 0x200;           // 16 bit program counter
char mem[4096] = {0};       // 4096 bytes of addressable memory (free memory starts at 0x200)
bool keyboard[16] = {0};     // array used for representing input

// FLAGS
bool vy_shift = false;      // if true, uses VY for 8XY6 and 8XYE

void tick() {
    char op = (mem[PC] >> 4) & 0b1111;      // first nibble of opcode
    char x = mem[PC] & 0b1111;              // second nibble of opcode
    char y = (mem[PC + 1] >> 4) & 0b1111;   // third nibble of opcode
    char n = mem[PC + 1] & 0b1111;          // fourth nibble of opcode
    char nn = mem[PC + 1];                  // second byte
    short nnn = (x << 4) | nn;              // 2nd. 3rd, and 4th nibble

    switch (op) {
        case 0x0:
            if (nn == 0xE0) {
                // CLS

            } else if (nn == 0xEE) {
                // RET
                PC = stack[s_pointer];
                s_pointer -= 1;
            }
            break;
        case 0x1:
            // JP addr
            PC = nnn;
            break;
        case 0x2:
            // CALL addr
            s_pointer += 1;
            stack[s_pointer] = PC;
            PC = nnn;
            break;
        case 0x3:
            // SE Vx, byte
            // skip if vx = nn
            if (V[x] = nn) {
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
            V[x] == nn;
            break;
        case 0x7:
            // ADD Vx, byte
            V[x] == V[x] + nn;
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
                    short sum = V[x] + V[y];
                    V[x] = (char) (sum && 0xFF);
                    if (sum > 0xFF) {
                        // Set overflow flag
                        V[0xF] = 0x01;
                    }
                    break;
                case 0x5:
                    // SUB Vx, Vy
                    if (V[x] < V[y]) {
                        V[0xF] = 0x01;
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
                        V[0xF] = 0x01;
                    }
                    V[x] = V[y] - V[x];
                    break;
                case 0xE:
                    // SHL Vx {, Vy}
                    if (vy_shift) {
                        V[x] = V[y];
                    }
                    V[0xF] = 0x01 & (V[x] >> 3);
                    V[x] = V[x] << 1;
                    break;
                default:
                    break;
            }
            break;
        case 0x9:
            break;
        case 0xA:
            break;
        case 0xB:
            break;
        case 0xC:
            break;
        case 0xD:
            break;
        case 0xE:
            break;
        case 0xF:
            break;
        default:
            break;
    }

    PC += 2;
}

int main() {
    tick();
    return 0;
}



