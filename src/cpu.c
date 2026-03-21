// This program emulates the instruction logic of the CHIP8 processor
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <time.h>

#include "cpu.h"

// All opcodes are of the form NNNN, where N is 4 bits
void cpu_execute(Chip8* c, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t kk = (opcode & 0x00FF);

    switch (opcode & 0xF000) { // Extracts the first bit to determine what type of instruction it is
        case 0x0000: {
            switch (opcode) {
                case 0x00E0: { // CLS: clear screen
                    memset(c->frame_buffer, 0, sizeof(c->frame_buffer));
                    c->pc += 2;
                    break;
                }

                case 0x00EE: { //RET: return from a subroutine
                    // The interpreter sets the program counter to the address at the top of the stack,
                    // then subtracts 1 from the stack pointer.
                    c->pc = c->stack[c->sp];
                    c->sp--;
                    c->pc += 2; // Move past the CALL instruction
                    break;
                }

                default: {
                    fprintf(stderr, "Unknown opcode: %04X\n", opcode);
                    exit(1);
                }
            }
        }
        
        case 0x1000: { // JP addr: Jump to location nnn
            // The interpreter sets the program counter to nnn
            c->pc = opcode & 0x0FFF;
            break;
        }
            
        case 0x2000: { // CALL addr: Call subroutine at nnn
            // The interpreter increments the stack pointer, then puts the current PC on the top
            // of the stack. The PC is then set to nnn.
            c->sp++;
            c->pc = opcode & 0x0FFF;
            break;
        }

        case 0x3000: { // SE Vx, byte: Skip next instruction if Vx = kk
            // The interpreter compares register Vx to kk, and if they are equal,
            // increments the program counter by 2.
            if(c->V[x] == kk) c->pc += 2;
            c->pc += 2;
            break;
        }

        case 0x4000: { // SNE Vx, byte: Skip next instruction if Vx \neq kk
            if(c->V[x] != kk) c->pc += 2;
            c->pc += 2;
            break;
        }

        case 0x5000: { // SE Vx, Vy: Skip next instruction if Vx = Vy
            if(c->V[x] == c->V[y]) c->pc += 2;
            c->pc += 2;
            break;
        }

        case 0x6000: { // LD Vx, byte: Set Vx = kk
            c->V[x] = kk;
            c->pc += 2;
            break;
        }

        case 0x7000: { // ADD Vx, byte: Set Vx = Vx + kk
            c->V[x] += kk;
            c->pc += 2;
            break;
        }
            
        case 0x8000: {
            switch(opcode & 0x000F) {
                case 0x0000: { // LD Vx, Vy: Set Vx = Vy
                    c->V[x] = c->V[y];
                    c->pc += 2;
                    break;
                }
                    
                case 0x0001: { // OR Vx, Vy: Set Vx = Vx OR Vy
                    c->V[x] = c->V[x] | c->V[y];
                    c->pc += 2;
                    break;
                }

                case 0x0002: { // AND Vx, Vy: Set Vx = Vx AND Vy
                    c->V[x] = c->V[x] & c->V[y];
                    c->pc += 2;
                    break;
                }

                case 0x0003: { // XOR Vx, Vy: Set Vx = Vx XOR Vy
                    c->V[x] = c->V[x] ^ c->V[y];
                    c->pc += 2;
                    break;
                }

                case 0x0004: { // ADD Vx, Vy: Set Vx = Vx + Vy, set VF = carry
                    // The values of Vx and Vy are added together. If the result is greater
                    // than 8 bits (i.e., ¿ 255,) VF is set to 1, otherwise 0. 
                    // Only the lowest 8 bits of the result are kept, and stored in Vx.
                    uint16_t sum = (uint16_t)c->V[x] + (uint16_t)c->V[y];
                    c->V[0xF] = (sum > 0xFF) ? 1 : 0;
                    c->V[x] = (uint8_t)sum;
                    c->pc += 2;
                    break;
                }

                case 0x0005: { // SUB Vx, Vy: Set Vx = Vx - Vy, set VF = NOT borrow
                    // If Vx ¿ Vy, then VF is set to 1, otherwise 0
                    // Then Vy is subtracted from Vx, and the results stored in Vx.
                    uint8_t Vx = c->V[x];
                    uint8_t Vy = c->V[y];

                    c->V[0xF] = (Vx >= Vy) ? 1 : 0;  // no borrow => 1, borrow => 0
                    c->V[x] = Vx - Vy;               // wraps naturally in uint8_t
                    c->pc += 2;
                    break;
                }

                case 0x0006: { // SHR Vx {, Vy}: Set Vx = Vx SHR 1
                    // If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0
                    // Then Vx is divided by 2.
                    c->V[0xF] = c->V[x] & 0x01;
                    c->V[x] >>= 1;
                    c->pc += 2;
                    break;
                }

                case 0x0007: { // SUBN Vx Vy: Set Vx = Vy - Vx, set VF = NOT borrow
                    // If Vy ¿ Vx, then VF is set to 1, otherwise 0. 
                    // Then Vx is subtracted from Vy, and the results stored in Vx.
                    uint8_t Vx = c->V[x];
                    uint8_t Vy = c->V[y];

                    c->V[0xF] = (Vy >= Vx) ? 1 : 0;  // no borrow => 1, borrow => 0
                    c->V[x] = Vy - Vx;
                    c->pc += 2;
                    break;
                }

                case 0x0008: { // SHL Vx {, Vy}: Set Vx = Vx SHL 1
                    // If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. 
                    // Then Vx is multiplied by 2.
                    c->V[0xF] = (c->V[x] & 0x80) >> 7;
                    c->V[x] <<= 1;
                    c->pc += 2;
                    break;
                }

                default: {
                    fprintf(stderr, "Unknown opcode: %04X\n", opcode);
                    exit(1);
                }
            }
        }

        case 0x9000: { // SNE Vx, Vy: Skip next instruction if Vx != Vy
            if(c->V[x] == c->V[y]) c->pc += 2;
            c->pc += 2;
            break;
        }

        case 0xA000: { // LD I, addr: Set I = nnn
            c->I = opcode & 0x0FFF;
            c-> pc += 2;
            break;
        }

        case 0xB000: { // JP V0, addr: Jump to location nnn + V0
            c->pc = c->V[0x0] + (opcode & 0x0FFF);
            break;
        }

        case 0xC000: { // Set Vx = random byte AND kk
            // The interpreter generates a random number from 0 to 255, which is then
            // ANDed with the value kk. The results are stored in Vx.
            c->V[x] = (uint8_t)(rand() % 256) & kk;
            c->pc += 2;
            break;
        }

        case 0xD000: { // DRW Vx, Vy, nibble: Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
            // The interpreter reads n bytes from memory, starting at the address stored in I. 
            // These bytes are then displayed as sprites on screen at coordinates (Vx, Vy). 
            // Sprites are XOR’d onto the existing screen. 
            // If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0.
            // If the sprite is positioned so part of it is outside the coordinates of the display, it wraps around to the opposite side of the screen.
            uint8_t n = opcode & 0x000F;
            uint8_t Vx = c->V[x];
            uint8_t Vy = c->V[y];

            c->V[0xF] = 0; // reset collision flag
            // TODO
        }

        case 0xE000: {
            switch(opcode & 0x00FF) {
                case 0x009E: { // SKP Vx: Skip next instruction if key with the value of Vx is pressed
                    uint8_t Vx = c->V[x];
                    if (c->keypad[Vx] == 1) c->pc += 2;
                    c->pc += 2;
                    break;
                }

                case 0x00A1: { // SKNP Vx: Skip next instruction if key with the value of Vx is not pressed.
                    uint8_t Vx = c->V[x];
                    if (c->keypad[Vx] == 0) c->pc += 2;
                    c->pc += 2;
                    break;
                }
                
                default: {
                    fprintf(stderr, "Unknown opcode: %04X\n", opcode);
                    exit(1);
                }
            }
        }

        case 0xF000: {
            switch(opcode & 0x00FF) {
                case 0x0007: { // LD Vx, DT: Set Vx = delay timer value
                    c->V[x] = c->delay_timer;
                    c->pc += 2;
                    break;
                }

                case 0x000A: { // LD Vx, K: Wait for a key press, store the value of the key in Vx
                    for (uint8_t i = 0; i < 16; i++) {
                        if (c->keypad[i] == 1) {
                            c->V[x] = i;
                            c->pc += 2;
                            break;
                        }
                    }
                    break;
                }

                case 0x0015: { // LD DT, Vx: Set delay timer = Vx
                    c->delay_timer = c->V[x];
                    c->pc += 2;
                }

                case 0x0018: { // LD ST, Vx: Set sound timer = Vx
                    c->sound_timer = c->V[x];
                    c->pc += 2;
                }

                case 0x001E: { // ADD I, Vx: Set I = I + Vx
                    c->I += c->V[x];
                    c->pc += 2;
                }
            }
        }
    }
}