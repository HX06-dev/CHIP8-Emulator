// This program is responsible for graphics
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "display.h"

// Buffer size:
// - ESC move-home: 3 bytes ("\033[H")
// - 32 rows * (64 pixels * 2 chars + '\n') = 32 * 129
// - NUL terminator
#define FRAME_CHARS (3 + (32 * (64 * 2 + 1)) + 1)

// optional: clear screen once at start
void display_init() {
    printf("\033[2J"); // clear entire screen
    fflush(stdout);
}

// move cursor to top-left + draw framebuffer
void display_draw(Chip8* c) {
    static char frame[FRAME_CHARS];
    size_t pos = 0;

    // Move cursor to top-left
    frame[pos++] = '\033';
    frame[pos++] = '[';
    frame[pos++] = 'H';

    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            int pixel = c->frame_buffer[x + y * 64];
            frame[pos++] = pixel ? '#' : ' ';
            frame[pos++] = pixel ? '#' : ' ';
        }
        frame[pos++] = '\n';
    }

    frame[pos] = '\0';

    // One call for the full frame to reduce terminal overhead.
    fwrite(frame, 1, pos, stdout);
    fflush(stdout);
}

// clear screen (optional utility)
void display_clear() {
    printf("\033[2J");
    fflush(stdout);
}