/*
 * Propeller Game Engine (C) 2013 Marco Maccaferri
 * Based on code written by Jeff Ledger / Oldbitcollector
 *
 * TERMS OF USE: MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
 * is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Contributors:
 *     Jeff Ledger / Oldbitcollector - initial API and implementation
 *     Marco Maccaferri - adapted for game engine usage
 */

#include "nes_gamepad_ctrl.h"

#define NES0_RIGHT  0x0001
#define NES0_LEFT   0x0002
#define NES0_DOWN   0x0004
#define NES0_UP     0x0008
#define NES0_START  0x0010
#define NES0_SELECT 0x0020
#define NES0_B      0x0040
#define NES0_A      0x0080

#define NES1_RIGHT  0x0100
#define NES1_LEFT   0x0200
#define NES1_DOWN   0x0400
#define NES1_UP     0x0800
#define NES1_START  0x1000
#define NES1_SELECT 0x2000
#define NES1_B      0x4000
#define NES1_A      0x8000

static uint16_t nes_bits;

void read_gamepad() {
    int i;

    // step 1: set I/Os
    DIRA |= (1 << JOY_CLK) | (1 << JOY_LCH);

    // step 2: set clock and latch to 0
    OUTA &= ~((1 << JOY_CLK) | (1 << JOY_LCH));

    // step 3: set latch to 1
    OUTA |= (1 << JOY_LCH);

    // step 4: set latch to 0
    OUTA &= ~(1 << JOY_LCH);

    // data is now ready to shift out, clear storage
    nes_bits = 0;

    // step 5: read 8 bits, 1st bits are already latched and ready, simply save and clock remaining bits
    for (i = 0; i < 8; i++) {
      nes_bits <<= 1;
      nes_bits |= ((INA >> JOY_DATAOUT0) & 1) | (((INA >> JOY_DATAOUT1) & 1) << 8);

      OUTA |= (1 << JOY_CLK); // JOY_CLK = 1
      OUTA &= ~(1 << JOY_CLK); // JOY_CLK = 0
    }

    // invert bits to make positive logic
    nes_bits = (~nes_bits & 0xFFFF);

    if ((nes_bits & 0xFF) == 0xFF)
      nes_bits = nes_bits & 0xFF00;

    if ((nes_bits & 0xFF00) == 0xFF00)
      nes_bits = nes_bits & 0xFF;
}

int player1_right() {
    return (nes_bits & NES0_RIGHT) != 0;
}

int player1_left() {
    return (nes_bits & NES0_LEFT) != 0;
}

int player1_up() {
    return (nes_bits & NES0_UP) != 0;
}

int player1_down() {
    return (nes_bits & NES0_DOWN) != 0;
}

int player1_select() {
    return (nes_bits & NES0_SELECT) != 0;
}

int player1_start() {
    return (nes_bits & NES0_START) != 0;
}

int player1_b() {
    return (nes_bits & NES0_B) != 0;
}

int player1_fire() {
    return (nes_bits & NES0_A) != 0;
}

int player1_none() {
    return (nes_bits & (NES0_RIGHT|NES0_LEFT|NES0_UP|NES0_DOWN|NES0_SELECT|NES0_START|NES0_B|NES0_A)) == 0;
}

int player2_right() {
    return (nes_bits & NES1_RIGHT) != 0;
}

int player2_left() {
    return (nes_bits & NES1_LEFT) != 0;
}

int player2_up() {
    return (nes_bits & NES1_UP) != 0;
}

int player2_down() {
    return (nes_bits & NES1_DOWN) != 0;
}

int player2_select() {
    return (nes_bits & NES1_SELECT) != 0;
}

int player2_start() {
    return (nes_bits & NES1_START) != 0;
}

int player2_b() {
    return (nes_bits & NES1_B) != 0;
}

int player2_fire() {
    return (nes_bits & NES1_A) != 0;
}

int player2_none() {
    return (nes_bits & (NES1_RIGHT|NES1_LEFT|NES1_UP|NES1_DOWN|NES1_SELECT|NES1_START|NES1_B|NES1_A)) == 0;
}
