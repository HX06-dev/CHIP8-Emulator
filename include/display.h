#ifndef DISPLAY_H
#define DISPLAY_H

#include "chip8.h"

// initialize display
void display_init();

// draw framebuffer to screen
void display_draw(Chip8* c);

// clear terminal screen
void display_clear();

#endif