/*
 * Propeller Game Engine (C) 2013 Marco Maccaferri
 *
 * AY3891X / YM2149F emulator
 * Based on AYcog by Johannes Ahlebrand
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
 *     Johannes Ahlebrand - Initial implementation
 *     Marco Maccaferri   - Converted to C and adapted for game engine
 */

#include <propeller.h>

#include "types.h"
#include "ay3891.h"

#define SAMPLE_RATE     0x0099
#define ARG1            0x009B
#define ARG2            0x009C
#define R1              0x009D

extern uint32_t binary_ay3891_dat_start[];

AY_REGISTERS registers;

AY_REGISTERS * ay3891_start(int16_t Right, int16_t Left) {
    int32_t cog;
    COGLONG * driver = (COGLONG *) binary_ay3891_dat_start;

    driver[ARG1].value = 0x18000000 | Left;
    driver[ARG2].value = 0x18000000 | Right;
    driver[R1].value = ((1 << Right) | (1 << Left)) & 0xFFFFFFFE;
    driver[SAMPLE_RATE].value = CLKFREQ / 125000;

    cog = cognew(driver, &registers);

    return cog != -1 ? &registers : NULL;
}

void ay3891_set_register(int8_t Reg, uint8_t Val) {
    registers.reg[Reg] = Val;
}

void ay3891_reset_registers() {
    memset(&registers, 0, sizeof(registers));
}

void ay3891_set_volume(uint8_t Channel, uint8_t Volumelevel) {
    registers.volume[Channel] = Volumelevel;
}

void ay3891_set_freq(uint8_t Channel, uint16_t Freq) {
    registers.freq[Channel]= Freq;
}

void ay3891_set_noise_freq(int32_t Freq) {
    registers.noise_period = Freq;
}

void ay3891_enable_tone(int8_t ch1, int8_t ch2, int8_t ch3) {
    registers.enable = (registers.enable & 0xF8) | ((~ch1)&1) | ((~ch2)&2) | ((~ch3)&4);
}

void ay3891_enable_noise(int8_t ch1, int8_t ch2, int8_t ch3) {
    registers.enable = (registers.enable & 0xC7) | ((~ch1)&8) | ((~ch2)&16) | ((~ch3)&32);
}

void ay3891_set_envelope(int8_t shape, uint16_t rate) {
    registers.envelope_shape = shape;
    registers.envelope_fine_tune = rate & 0xFF;
    registers.envelope_coarse_tune = rate >> 8;
}

void ay3891_trigger_envelope() {
    registers.envelope_shape = registers.envelope_shape & 0x0F;
}
