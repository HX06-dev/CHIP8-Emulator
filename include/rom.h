#ifndef ROM_H
#define ROM_H

#include "chip8.h"

// returns 0 on success, non-zero on failure
int rom_load(Chip8* c, const char* filename);

#endif