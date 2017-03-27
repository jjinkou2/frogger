/*
 * Propeller Game Engine (C) 2013 Marco Maccaferri
 * Based on code written by Marko Lukat
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
 *     Marko Lukat - initial implementation
 *     Marco Maccaferri - adapted for game engine usage
 */

#include "vga_driver.h"

#define VGA_BLNK        0x0127
#define VGA_BASE        0x0128
#define VGA_VRES        0x0129

extern uint32_t binary_vga_driver_dat_start[];

uint32_t scanline_buffer[64];
volatile uint32_t line_indicator;

void start_vga_driver() {
    COGLONG * driver = (COGLONG *) binary_vga_driver_dat_start;

    memset(&scanline_buffer, 0x00, sizeof(scanline_buffer));

    driver[VGA_BLNK].value = (uint32_t) &line_indicator;
    driver[VGA_BASE].value = (uint32_t) &scanline_buffer;
    driver[VGA_VRES].value = VRES;
    cognew(driver, 0);

    waitcnt(CNT + CLKFREQ);
}

void wait_vbl() {
    while (line_indicator == VRES)
        ;
    while (line_indicator != VRES)
        ;
}
