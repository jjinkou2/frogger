/*
 * Just a buch of function to work with the Memsic2125 dual axis
 * It replaces the nes_gamepad_ctrl.c
 * Based on code written by Marco Maccaferri
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
 */

#include "mx2125.h"                               // Include mx2125 header 


#define NES0_RIGHT  0x0001
#define NES0_LEFT   0x0002
#define NES0_DOWN   0x0004
#define NES0_UP     0x0008
#define NES0_START  0x0010
#define NES0_SELECT 0x0020
#define NES0_B      0x0040
#define NES0_A      0x0080


static uint16_t nes_bits;

void read_gamepad() {
    int x = mx_tilt(6);                          // X-axis tilt
    int y = mx_tilt(7);                          // Y-axis tilt
    int button_B = input(1);           // 2 push buttons connected to 
    int button_A = input(0);          // Pin 0 and 1
    
    // data is now ready to shift out, clear storage
    nes_bits = 0;
       
    if (y > 32)  nes_bits |= NES0_RIGHT;
    if (y < -32) nes_bits |= NES0_LEFT;
    if (x > 32)  nes_bits |= NES0_DOWN;
    if (x < -32) nes_bits |= NES0_UP;
    
    if (button_B==1) nes_bits |= NES0_B|NES0_SELECT;
    if (button_A==1) nes_bits |= NES0_A|NES0_START;
    
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
