#include <stdio.h>
#include "sound.h"

void sound_update(Chip8* c) {
    if (c->sound_timer > 0) {
        printf("\a"); // system beep
    }
}