// This program emulates the instruction logic of the CHIP8 processor
#include <stdio.h>
#include <string.h> 

#include "cpu.h"

// All opcodes are of the form NNNN, where N is 4 bits
void cpu_execute(Chip8* c, uint16_t opcode) {
    int x = (opcode & 0x0F00) >> 8;
    int y = (opcode & 0x00F0) >> 4;
    int kk = (opcode & 0x00FF);
    switch (opcode & 0xF000) { // Extracts the first bit to determine what type of instruction it is
        case 0x0000:
            switch (opcode) {
                case 0x00E0: // CLS: clear screen
                    memset(c->frame_buffer, 0, sizeof(c->frame_buffer));
                    c->pc += 2;
                    break;

                case 0x00EE: //RET: return from a subroutine
                    // The interpreter sets the program counter to the address at the top of the stack,
                    // then subtracts 1 from the stack pointer.
                    c->pc = c->stack[c->sp];
                    c->sp--;
                    c->pc += 2; // Move past the CALL instruction
                    break;

                default:
                    fprintf(stderr, "Unknown opcode: %04X\n", opcode);
                    break;

            }
        
            case 0x1000: // JP addr: Jump to location nnn
                // The interpreter sets the program counter to nnn
                c->pc = opcode & 0x0FFF;
                break;
            
            case 0x2000: // CALL addr: Call subroutine at nnn
                // The interpreter increments the stack pointer, then puts the current PC on the top
                // of the stack. The PC is then set to nnn.
                c->sp++;
                c->pc = opcode & 0x0FFF;
                break;

            case 0x3000: // SE Vx, byte: Skip next instruction if Vx = kk
                // The interpreter compares register Vx to kk, and if they are equal,
                // increments the program counter by 2.
                if(c->V[x] == kk) c->pc += 2;
                c->pc += 2;
                break;

            case 0x4000: // SNE Vx, byte: Skip next instruction if Vx \neq kk
                if(c->V[x] != kk) c->pc += 2;
                c->pc += 2;
                break;

            case 0x5000: // SE Vx, Vy: Skip next instruction if Vx = Vy
                if(c->V[x] == c->V[y]) c->pc += 2;
                c->pc += 2;
                break;

            case 0x6000: // LD Vx, byte: Set Vx = kk
                c->V[x] = kk;
                c->pc += 2;
                break;

            case 0x7000: // ADD Vx, byte: Set Vx = Vx + kk
                c->V[x] += kk;
                c->pc += 2;
                break;
            
            case 0x8000:
                switch(opcode & 0x000F) {
                    case 0x0000: // LD Vx, Vy: Set Vx = Vy
                        c->V[x] = c->V[y];
                        c->pc += 2;
                        break;
                    
                    case 0x0001: // OR Vx, Vy: Set Vx = Vx OR Vy
                        c->V[x] = c->V[x] | c->V[y];
                        c->pc += 2;
                        break;

                    case 0x0002: // AND Vx, Vy: Set Vx = Vx AND Vy
                        c->V[x] = c->V[x] & c->V[y];
                        c->pc += 2;
                        break;

                    case 0x0003: // XOR Vx, Vy: Set Vx = Vx XOR Vy
                        c->V[x] = c->V[x] ^ c->V[y];
                        c->pc += 2;
                        break;

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
                        uint8_t vx = c->V[x];
                        uint8_t vy = c->V[y];

                        c->V[0xF] = (vx >= vy) ? 1 : 0;  // no borrow => 1, borrow => 0
                        c->V[x] = vx - vy;               // wraps naturally in uint8_t
                        c->pc += 2;
                        break;
                    }

                    case 0x0006: // SHR Vx {, Vy}: Set Vx = Vx SHR 1
                        // If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0
                        // Then Vx is divided by 2.

                }


    }
}