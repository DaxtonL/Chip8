#include <stdio.h>
#include <stdbool.h>

char V[8] = {0};            // 8 bit registers V0-VF
short index_r = 0;          // 16 bit index register
char stack[64] = {0};       // 64 byte stack
char pointer = 0;           // 8 bit stack pointer
char delay_t = 0;           // 8 bit delay timer
char sound_t = 0;           // 8 bit sound timer
char fbuff[256] = {0};      // 64x32 bit frame buffer, stored as 256 bytes
short PC = 0x200;           // 16 bit program counter
char mem[4096] = {0};       // 4096 bytes of addressable memory (free memory starts at 0x200)
bool keyboard[16] = {0};     // array used for representing input

void tick() {
    char op = (mem[PC] >> 4) & 0b1111;      // first nibble of opcode
    char x = mem[PC] & 0b1111;              // second nibble of opcode
    char y = (mem[PC + 1] >> 4) & 0b1111;   // third nibble of opcode
    char n = mem[PC + 1] & 0b1111;          // fourth nibble of opcode
    char nn = mem[PC + 1];                  // second byte
    short nnn = (x << 4) | nn;              // 2nd. 3rd, and 4th nibble

    switch (op) {
        case 0x0:
            break;
        case 0x1:
            break;
        case 0x2:
            break;
        case 0x3:
            break;
        case 0x4:
            break;
        case 0x5:
            break;
        case 0x6:
            break;
        case 0x7:
            break;
        case 0x8:
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



