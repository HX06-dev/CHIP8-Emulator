// This program is responsible for loading programs
// Since rom files for CHIP8 are just binary, there is no metadata to worry about
#include <stdio.h>

#include "rom.h"
#include "chip8.h"

int rom_load(Chip8* c, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        perror("fread");
        return 1;
    }

    // Check if ROM is too big
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    if (size > (4096 - 0x200)) {
        fprintf(stderr, "ROM too large: %ld bytes\n", size);
        fclose(file);
        return 1;
    }
    rewind(file);

    // Starts reading ROM into chip memory
    fread(&c->memory[0x200], 1, size, file);
    
    // Cleans up and terminates
    fclose(file);
    return 0;

}