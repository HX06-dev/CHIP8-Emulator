// This program is responsible for handling keyboard input
#include <conio.h> // Use termios if on UNIX
#include <ctype.h>

#include "input.h"

// map keyboard -> CHIP-8 keypad
static uint8_t map_key(char key) {
    key = (char)tolower((unsigned char)key);

    if (key >= '0' && key <= '9') {
        return (uint8_t)(key - '0');
    }
    if (key >= 'a' && key <= 'f') {
        return (uint8_t)(10 + (key - 'a'));
    }
    return 0xFF;
}

// update keypad state
void input_update(Chip8* c) {
    for (int i = 0; i < 16; i++) {
        c->keypad[i] = 0;
    }

    while (_kbhit()) {
        int ch = _getch();

        // Ignore extended key prefixes for arrows/F-keys
        if (ch == 0 || ch == 0xE0) {
            (void)_getch();
            continue;
        }

        uint8_t key = map_key((char)ch);
        if (key != 0xFF) {
            c->keypad[key] = 1;
        }
    }
}