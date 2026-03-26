// This program is the main executable that ties all the other files together
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include "chip8.h"
#include "rom.h"
#include "sound.h"
#include "input.h"
#include "display.h"

int main(int argc, char* argv[]) {
    // This program expects 1 argument, being the filename of the ROM
    if (argc != 2) {
        printf("Usage: %s <ROM>\n", argv[0]);
        return 1;
    }

    if (strlen(argv[1]) > 100) {
        printf("ROM name is too long");
        return 1;
    }

    Chip8 chip8;
    chip8_init(&chip8);

    char r[107] = "../roms/";
    char* filepath = strcat(r, argv[1]);

    printf("%s\n", filepath);

    if (rom_load(&chip8, filepath) != 0) {
        perror("Invalid ROM");
        return 1;
    }

    int running = 1;

    clock_t last_timer = clock();
    clock_t last_draw = clock();

    while (running) {
        input_update(&chip8);

        // Run a small batch of CPU cycles each loop so emulation remains responsive.
        for (int i = 0; i < 10; i++) {
            chip8_cycle(&chip8);
        }

        clock_t now = clock();

        // Timers must tick at 60Hz.
        if ((double)(now - last_timer) / CLOCKS_PER_SEC >= (1.0 / 60.0)) {
            chip8_update_timers(&chip8);
            sound_update(&chip8);
            last_timer = now;
        }

        // Draw at 60Hz instead of every loop to reduce render/input stutter.
        if ((double)(now - last_draw) / CLOCKS_PER_SEC >= (1.0 / 60.0)) {
            display_draw(&chip8);
            last_draw = now;
        }

        // Throttle busy-loop CPU usage and smooth out input handling.
        usleep(1000);
    }

    return 0;
}