/*
 * Propeller Game Engine (C) 2013 Marco Maccaferri
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
 *     Marco Maccaferri - initial implementation
 */

#ifndef AY3891_H_
#define AY3891_H_

/*
  Reg bits function
  -----------------------------------
  00  7..0 channel A fine tune
  01  3..0 channel A coarse tune
  02  7..0 channel B fine tune
  03  3..0 channel B coarse tune
  04  7..0 channel C fine tune
  05  3..0 channel C coarse tune
  06  4..0 noise period
  07  7..0 enable register
  08  4..0 channel A volume
  09  4..0 channel B volume
  10  4..0 channel C volume
  11  7..0 envelope fine tune
  12  7..0 envelope coarse tune
  13  3..0 envelope shape
  14  7..0 I/O port A value
  15  7..0 I/O port B value
*/

typedef union {
    uint8_t reg[16];
    struct {
        uint16_t freq[3];
        uint8_t  noise_period;
        uint8_t  enable;
        uint8_t  volume[3];
        uint8_t  envelope_fine_tune;
        uint8_t  envelope_coarse_tune;
        uint8_t  envelope_shape;
        uint8_t  io_a;
        uint8_t  io_b;
    };
} AY_REGISTERS;

/**
 * Starts AYcog in a single cog
 *
 * Returns a pointer to the first AY register in hub memory
 * on success; otherwise returns NULL.
 *
 * right - The pin to output the right channel to. 0 = Not used
 * left - The pin to output the left channel to. 0 = Not used
 */
AY_REGISTERS * ay3891_start(int16_t Right, int16_t Left);

/**
 * Sets a single AY register to a value
 *
 * reg - The AY register to set
 * val - The value to set the register to
 */
void ay3891_set_register(int8_t Reg, uint8_t Val);

/**
 * Reset all 16 AY registers
 */
void ay3891_reset_registers();

/**
 * Sets the volume of an AY channel
 *
 * channel - The AY channel to set. (0 - 2)
 * volumeLevel - A value betwen 0 and 15.
 *               (A volumeLevel of 16 enables the envelope on the channel)
 */
void ay3891_set_volume(uint8_t Channel, uint8_t Volumelevel);

/**
 * Sets the frequency of an AY channel
 *
 * channel - The AY channel to set. (0 - 2)
 * freq - The 12 bit frequency value. (0 - 4095)
 */
void ay3891_set_freq(uint8_t Channel, uint16_t Freq);

/**
 * Sets the frequency of the noise generator
 *
 * freq - The 5 bit frequency value. (0 - 31)
 *
 * Frequency in Hz = PSG_FREQ / 64 / freq
 */
void ay3891_set_noise_freq(int32_t Freq);

/**
 * Enable/Disable tone on AY channels
 *
 * ch1 - Enable/Disable tone on ch 1. (True/False)
 * ch2 - Enable/Disable tone on ch 2. (True/False)
 * ch3 - Enable/Disable tone on ch 3. (True/False)
 */
void ay3891_enable_tone(int8_t ch1, int8_t ch2, int8_t ch3);

/**
 * Enable/Disable noise on channels
 *
 * ch1 - Enable/Disable noise on ch 1. (True/False)
 * ch2 - Enable/Disable noise on ch 2. (True/False)
 * ch3 - Enable/Disable noise on ch 3. (True/False)
 */
void ay3891_enable_noise(int8_t ch1, int8_t ch2, int8_t ch3);

/**
 * Sets the properties of the envelope generator
 *
 * shape - The 4 bit shape value. (See table below)
 * rate - The 16 bit envelope rate value. (0 - 65535)
 *
 *   Bits     Shape     Description               Hold
 * --------   -----     -----------               ----
 *
 *  0 0 x x   \___      High to low               Low
 *
 *  0 1 x x   /___      Low to high               Low
 *
 *  1 0 0 0   \\\\      High to low ...           Loop
 *
 *  1 0 0 1   \___      High to low               Low
 *
 *  1 0 1 0   \/\/      High to low to high ...   Loop
 *
 *  1 0 1 1   \"""      High to low               High
 *
 *  1 1 0 0   ////      Low to high ...           Loop
 *
 *  1 1 0 1   /"""      Low to high               High
 *
 *  1 1 1 0   /\/\      Low to high to low  ...   Loop
 *
 *  1 1 1 1   /___      Low to high               Low
 *
 *  Envelope step frequency = PSG_FREQ / 64 / rate
 *  (The envelope generator has 16 steps)
 *
 *  NOTE: To enable the envelope on an AY channel, set volume to 16
 */
void ay3891_set_envelope(int8_t shape, uint16_t rate);

/**
 * Trigger the envelope generator
 */
void ay3891_trigger_envelope();

#endif /* AY3891_H_ */
