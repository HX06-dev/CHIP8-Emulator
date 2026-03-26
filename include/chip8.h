#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

typedef struct {
    uint8_t memory[4096]; //4096 bytes of addressable memory
    uint8_t V[16]; // sixteen 8-bit registers
    uint16_t I; // 16-bit index register
    uint16_t pc; // 16-bit program counter

    uint16_t stack[16]; // 64-bit stack
    uint8_t sp; // 8 bit stack pointer

    uint8_t delay_timer; // 8-bit delay timer
    uint8_t sound_timer; // 8-bit sound timer

    uint8_t frame_buffer[64 * 32]; //64x32 frame_buffer
    uint8_t keypad[16];

} Chip8;

void chip8_init(Chip8* c); // Initializes the processor
void chip8_cycle(Chip8* c); // Simulates a cycle
void chip8_update_timers(Chip8* c); // Update the timers

#endif