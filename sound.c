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

#include <stdlib.h>
#include <stdint.h>

#include "ay3891.h"

typedef struct {
    unsigned char  * ptr;
    unsigned char   idx;
    unsigned char   offset;
    unsigned char   volume;
    unsigned char   volume_reload;
    unsigned char   length;
} VOICE;

typedef union {
    unsigned char value;
    struct {
        unsigned char n: 5;
        unsigned char c: 3;
    };
} SONG_DATA;

#define CMD     0x1F
#define REST    0x00

uint16_t time_table[] = {
    1, 2, 4, 8, 16-4, 32-8, 64-16, 128-32
};

uint16_t note_table[] = {
    0x086B,   //   51.90 1G# - 0
    0x07F2,   //   54.98 1A  - 1
    0x0780,   //   58.25 1A# - 2
    0x0714,   //   61.72 1B  - 3
    0x06AE,   //   65.40 2C  - 4
    0x064E,   //   69.29 2C# - 5
    0x05F3,   //   73.43 2D  - 6
    0x059E,   //   77.78 2D# - 7
    0x054E,   //   82.36 2E  - 8
    0x0501,   //   87.31 2F  - 9
    0x04B9,   //   92.51 2F# - 10
    0x0476,   //   97.94 2G  - 11
    0x0436,   //  103.75 2G# - 12
    0x03F9,   //  109.97 2A  - 13
    0x03C0,   //  116.50 2A# - 14
    0x038A,   //  123.45 2B  - 15
    0x0357,   //  130.81 3C  - 16
    0x0327,   //  138.59 3C# - 17
    0x02FA,   //  146.78 3D  - 18
    0x02CF,   //  155.56 3D# - 19
    0x02A7,   //  164.72 3E  - 20
    0x0281,   //  174.49 3F  - 21
    0x025D,   //  184.87 3F# - 22
    0x023B,   //  195.88 3G  - 23
    0x021B,   //  207.51 3G# - 24
    0x01FD,   //  219.74 3A  - 25
    0x01E0,   //  233.01 3A# - 26
    0x01C5,   //  246.90 3B  - 27
    0x01AC,   //  261.32 4C  - 28
    0x0194,   //  276.85 4C# - 29
    0x017D,   //  293.56 4D  - 30
    0x0168,   //  310.68 4D# - 31
    0x0153,   //  329.93 4E  - 32
    0x0140,   //  349.52 4F  - 33
    0x012E,   //  370.35 4F# - 34
    0x011D,   //  392.44 4G  - 35
    0x010D,   //  415.79 4G# - 36
    0x00FE,   //  440.34 4A  - 37
    0x00F0,   //  466.03 4A# - 38
    0x00E3,   //  492.72 4B  - 39
    0x00D6,   //  522.65 5C  - 40
    0x00CA,   //  553.70 5C# - 41
    0x00BE,   //  588.67 5D  - 42
    0x00B4,   //  621.37 5D# - 43
    0x00AA,   //  657.93 5E  - 44
    0x00A0,   //  699.05 5F  - 45
    0x0097,   //  740.71 5F# - 46
    0x008F,   //  782.15 5G  - 47
    0x0087,   //  828.50 5G# - 48
    0x007F,   //  880.69 5A  - 49
    0x0078,   //  932.06 5A# - 50
    0x0071,   //  989.80 5B  - 51
    0x006B,   // 1045.3  6C  - 52
    0x0065,   // 1107.4  6C# - 53
    0x005F,   // 1177.3  6D  - 54
    0x005A,   // 1242.7  6D# - 55
    0x0055,   // 1315.8  6E  - 56
    0x0050,   // 1398.1  6F  - 57
    0x004C,   // 1471.6  6F# - 58
    0x0047,   // 1575.3  6G  - 59
};

uint8_t intro_voice_a[] = {
    0x07,       // NOTE 4C# for 2^0
    0x91,       // NOTE 4B for 2^4
    0x8D,       // NOTE 4G for 2^4
    0x8D,       // NOTE 4G for 2^4
    0x8D,       // NOTE 4G for 2^4
    0x91,       // NOTE 4B for 2^4
    0x8D,       // NOTE 4G for 2^4
    0x8D,       // NOTE 4G for 2^4
    0x8D,       // NOTE 4G for 2^4
    0x92,       // NOTE 5C for 2^4
    0x92,       // NOTE 5C for 2^4
    0x91,       // NOTE 4B for 2^4
    0x91,       // NOTE 4B for 2^4
    0xAF,       // NOTE 4A for 2^5
    0xA0,       // REST for 2^5
    0x92,       // NOTE 5C for 2^4
    0x92,       // NOTE 5C for 2^4
    0x91,       // NOTE 4B for 2^4
    0x91,       // NOTE 4B for 2^4
    0x8F,       // NOTE 4A for 2^4
    0x8F,       // NOTE 4A for 2^4
    0x96,       // NOTE 5E for 2^4
    0x96,       // NOTE 5E for 2^4
    0x94,       // NOTE 5D for 2^4
    0x92,       // NOTE 5C for 2^4
    0x91,       // NOTE 4B for 2^4
    0x8F,       // NOTE 4A for 2^4
    0xAD,       // NOTE 4G for 2^5
    0xA0,       // REST for 2^5
    0xFF,       // END OF VOICE
};

uint8_t intro_voice_b[] = {
    0x07,       // NOTE 3C# for 2^0
    0x8D,       // NOTE 3G for 2^4
    0x91,       // NOTE 3B for 2^4
    0x88,       // NOTE 3D for 2^4
    0x91,       // NOTE 3B for 2^4
    0x8D,       // NOTE 3G for 2^4
    0x91,       // NOTE 3B for 2^4
    0x88,       // NOTE 3D for 2^4
    0x91,       // NOTE 3B for 2^4
    0x8F,       // NOTE 3A for 2^4
    0x92,       // NOTE 4C for 2^4
    0x88,       // NOTE 3D for 2^4
    0x92,       // NOTE 4C for 2^4
    0x8F,       // NOTE 3A for 2^4
    0x92,       // NOTE 4C for 2^4
    0x88,       // NOTE 3D for 2^4
    0x92,       // NOTE 4C for 2^4
    0x8F,       // NOTE 3A for 2^4
    0x92,       // NOTE 4C for 2^4
    0x88,       // NOTE 3D for 2^4
    0x92,       // NOTE 4C for 2^4
    0x8F,       // NOTE 3A for 2^4
    0x92,       // NOTE 4C for 2^4
    0x88,       // NOTE 3D for 2^4
    0x92,       // NOTE 4C for 2^4
    0x8F,       // NOTE 3A for 2^4
    0x92,       // NOTE 4C for 2^4
    0x88,       // NOTE 3D for 2^4
    0x92,       // NOTE 4C for 2^4
    0xB1,       // NOTE 3B for 2^5
    0xA0,       // REST for 2^5
    0xFF,       // END OF VOICE
};

uint8_t intro_voice_c[] = {
    0x07,       // NOTE 3C# for 2^0
    0x80,       // REST for 2^4
    0x8D,       // NOTE 3G for 2^4
    0x80,       // REST for 2^4
    0x8D,       // NOTE 3G for 2^4
    0x80,       // REST for 2^4
    0x8D,       // NOTE 3G for 2^4
    0x80,       // REST for 2^4
    0x8D,       // NOTE 3G for 2^4
    0x80,       // REST for 2^4
    0x8F,       // NOTE 3A for 2^4
    0x80,       // REST for 2^4
    0x8F,       // NOTE 3A for 2^4
    0x80,       // REST for 2^4
    0x8F,       // NOTE 3A for 2^4
    0x80,       // REST for 2^4
    0x8F,       // NOTE 3A for 2^4
    0x80,       // REST for 2^4
    0x8F,       // NOTE 3A for 2^4
    0x80,       // REST for 2^4
    0x8F,       // NOTE 3A for 2^4
    0x80,       // REST for 2^4
    0x8F,       // NOTE 3A for 2^4
    0x80,       // REST for 2^4
    0x8F,       // NOTE 3A for 2^4
    0x80,       // REST for 2^4
    0x8F,       // NOTE 3A for 2^4
    0x80,       // REST for 2^4
    0x8F,       // NOTE 3A for 2^4
    0xAD,       // NOTE 3G for 2^5
    0xA0,       // REST for 2^5
    0xFF,       // END OF VOICE
};

unsigned char main_a[] = {
    0x05,       // NOTE 3B for 2^0
    0xB4,       // NOTE 5D for 2^5
    0x91,       // NOTE 4B for 2^4
    0x8D,       // NOTE 4G for 2^4
    0xB9,       // NOTE 5G for 2^5
    0x98,       // NOTE 5F# for 2^4
    0x96,       // NOTE 5E for 2^4
    0xB4,       // NOTE 5D for 2^5
    0x99,       // NOTE 5G for 2^4
    0x91,       // NOTE 4B for 2^4
    0x8F,       // NOTE 4A for 2^4
    0xB4,       // NOTE 5D for 2^5
    0x80,       // REST for 2^4
    0x80,       // REST for 2^4
    0x94,       // NOTE 5D for 2^4
    0x94,       // NOTE 5D for 2^4
    0x94,       // NOTE 5D for 2^4
    0x94,       // NOTE 5D for 2^4
    0x91,       // NOTE 4B for 2^4
    0x8F,       // NOTE 4A for 2^4
    0x8D,       // NOTE 4G for 2^4
    0x80,       // REST for 2^4
    0x99,       // NOTE 5G for 2^4
    0x99,       // NOTE 5G for 2^4
    0x99,       // NOTE 5G for 2^4
    0x9B,       // NOTE 5A for 2^4
    0x99,       // NOTE 5G for 2^4
    0x98,       // NOTE 5F# for 2^4
    0x96,       // NOTE 5E for 2^4
    0x94,       // NOTE 5D for 2^4
    0x91,       // NOTE 4B for 2^4
    0x80,       // REST for 2^4
    0x91,       // NOTE 4B for 2^4
    0xB9,       // NOTE 5G for 2^5
    0xB1,       // NOTE 4B for 2^5
    0x94,       // NOTE 5D for 2^4
    0xCF,       // NOTE 4A for 2^6
    0x80,       // REST for 2^4
    0xA0,       // REST for 2^5
    0x80,       // REST for 2^4
    0x91,       // NOTE 4B for 2^4
    0x91,       // NOTE 4B for 2^4
    0x92,       // NOTE 5C for 2^4
    0x94,       // NOTE 5D for 2^4
    0x96,       // NOTE 5E for 2^4
    0x98,       // NOTE 5F# for 2^4
    0xD6,       // NOTE 5E for 2^6
    0xC0,       // REST for 2^6
    0x80,       // REST for 2^4
    0x92,       // NOTE 5C for 2^4
    0x92,       // NOTE 5C for 2^4
    0x94,       // NOTE 5D for 2^4
    0xB6,       // NOTE 5E for 2^5
    0x98,       // NOTE 5F# for 2^4
    0x99,       // NOTE 5G for 2^4
    0xD8,       // NOTE 5F# for 2^6
    0xA0,       // REST for 2^5
    0xB4,       // NOTE 5D for 2^5
    0xD9,       // NOTE 5G for 2^6
    0x99,       // NOTE 5G for 2^4
    0x98,       // NOTE 5F# for 2^4
    0x96,       // NOTE 5E for 2^4
    0x94,       // NOTE 5D for 2^4
    0xD8,       // NOTE 5F# for 2^6
    0xB6,       // NOTE 5E for 2^5
    0xB6,       // NOTE 5E for 2^5
    0xB4,       // NOTE 5D for 2^5
    0xBB,       // NOTE 5A for 2^5
    0xB9,       // NOTE 5G for 2^5
    0xB8,       // NOTE 5F# for 2^5
    0xD9,       // NOTE 5G for 2^6
    0xC0,       // REST for 2^6
    0x99,       // NOTE 5G for 2^4
    0x99,       // NOTE 5G for 2^4
    0x99,       // NOTE 5G for 2^4
    0x99,       // NOTE 5G for 2^4
    0x99,       // NOTE 5G for 2^4
    0x99,       // NOTE 5G for 2^4
    0x98,       // NOTE 5F# for 2^4
    0x96,       // NOTE 5E for 2^4
    0xD9,       // NOTE 5G for 2^6
    0xB4,       // NOTE 5D for 2^5
    0x91,       // NOTE 4B for 2^4
    0x91,       // NOTE 4B for 2^4
    0xAF,       // NOTE 4A for 2^5
    0x8F,       // NOTE 4A for 2^4
    0x8F,       // NOTE 4A for 2^4
    0x99,       // NOTE 5G for 2^4
    0x99,       // NOTE 5G for 2^4
    0x98,       // NOTE 5F# for 2^4
    0x96,       // NOTE 5E for 2^4
    0xD6,       // NOTE 5E for 2^6
    0xD4,       // NOTE 5D for 2^6
    0x94,       // NOTE 5D for 2^4
    0x91,       // NOTE 4B for 2^4
    0x91,       // NOTE 4B for 2^4
    0x91,       // NOTE 4B for 2^4
    0xB1,       // NOTE 4B for 2^5
    0x8F,       // NOTE 4A for 2^4
    0x8D,       // NOTE 4G for 2^4
    0x92,       // NOTE 5C for 2^4
    0x91,       // NOTE 4B for 2^4
    0x92,       // NOTE 5C for 2^4
    0x94,       // NOTE 5D for 2^4
    0xB6,       // NOTE 5E for 2^5
    0xA0,       // REST for 2^5
    0x94,       // NOTE 5D for 2^4
    0x92,       // NOTE 5C for 2^4
    0x8F,       // NOTE 4A for 2^4
    0x8F,       // NOTE 4A for 2^4
    0xAF,       // NOTE 4A for 2^5
    0x8D,       // NOTE 4G for 2^4
    0x8C,       // NOTE 4F# for 2^4
    0x8D,       // NOTE 4G for 2^4
    0x8C,       // NOTE 4F# for 2^4
    0x8D,       // NOTE 4G for 2^4
    0x8F,       // NOTE 4A for 2^4
    0xD1,       // NOTE 4B for 2^6
    0x94,       // NOTE 5D for 2^4
    0x91,       // NOTE 4B for 2^4
    0x91,       // NOTE 4B for 2^4
    0x91,       // NOTE 4B for 2^4
    0xB1,       // NOTE 4B for 2^5
    0x8F,       // NOTE 4A for 2^4
    0x8D,       // NOTE 4G for 2^4
    0x92,       // NOTE 5C for 2^4
    0x91,       // NOTE 4B for 2^4
    0x92,       // NOTE 5C for 2^4
    0x94,       // NOTE 5D for 2^4
    0xB6,       // NOTE 5E for 2^5
    0x98,       // NOTE 5F# for 2^4
    0x96,       // NOTE 5E for 2^4
    0xB4,       // NOTE 5D for 2^5
    0x94,       // NOTE 5D for 2^4
    0x96,       // NOTE 5E for 2^4
    0x94,       // NOTE 5D for 2^4
    0x92,       // NOTE 5C for 2^4
    0x91,       // NOTE 4B for 2^4
    0x8F,       // NOTE 4A for 2^4
    0xCA,       // NOTE 4E for 2^6
    0xAC,       // NOTE 4F# for 2^5
    0xAF,       // NOTE 4A for 2^5
    0xCD,       // NOTE 4G for 2^6
    0xC0,       // REST for 2^6
    0xFF,       // END OF VOICE
};

unsigned char main_b[] = {
    0x05,       // NOTE 2B for 2^0
    0xD9,       // NOTE 4G for 2^6
    0xD6,       // NOTE 4E for 2^6
    0xD9,       // NOTE 4G for 2^6
    0xD8,       // NOTE 4F# for 2^6
    0x8D,       // NOTE 3G for 2^4
    0x91,       // NOTE 3B for 2^4
    0x88,       // NOTE 3D for 2^4
    0x91,       // NOTE 3B for 2^4
    0x8D,       // NOTE 3G for 2^4
    0x91,       // NOTE 3B for 2^4
    0x88,       // NOTE 3D for 2^4
    0x91,       // NOTE 3B for 2^4
    0x8D,       // NOTE 3G for 2^4
    0x92,       // NOTE 4C for 2^4
    0x8A,       // NOTE 3E for 2^4
    0x92,       // NOTE 4C for 2^4
    0x8D,       // NOTE 3G for 2^4
    0x92,       // NOTE 4C for 2^4
    0x8A,       // NOTE 3E for 2^4
    0x92,       // NOTE 4C for 2^4
    0x8D,       // NOTE 3G for 2^4
    0x91,       // NOTE 3B for 2^4
    0x88,       // NOTE 3D for 2^4
    0x91,       // NOTE 3B for 2^4
    0x8D,       // NOTE 3G for 2^4
    0x91,       // NOTE 3B for 2^4
    0x88,       // NOTE 3D for 2^4
    0x91,       // NOTE 3B for 2^4
    0x8F,       // NOTE 3A for 2^4
    0x94,       // NOTE 4D for 2^4
    0x88,       // NOTE 3D for 2^4
    0x94,       // NOTE 4D for 2^4
    0x8A,       // NOTE 3E for 2^4
    0x94,       // NOTE 4D for 2^4
    0x8C,       // NOTE 3F# for 2^4
    0x94,       // NOTE 4D for 2^4
    0x8D,       // NOTE 3G for 2^4
    0x91,       // NOTE 3B for 2^4
    0x88,       // NOTE 3D for 2^4
    0x91,       // NOTE 3B for 2^4
    0x8D,       // NOTE 3G for 2^4
    0x91,       // NOTE 3B for 2^4
    0x88,       // NOTE 3D for 2^4
    0x91,       // NOTE 3B for 2^4
    0x8D,       // NOTE 3G for 2^4
    0x92,       // NOTE 4C for 2^4
    0x8A,       // NOTE 3E for 2^4
    0x92,       // NOTE 4C for 2^4
    0x8D,       // NOTE 3G for 2^4
    0x92,       // NOTE 4C for 2^4
    0x8A,       // NOTE 3E for 2^4
    0x92,       // NOTE 4C for 2^4
    0x8D,       // NOTE 3G for 2^4
    0x92,       // NOTE 4C for 2^4
    0x8A,       // NOTE 3E for 2^4
    0x92,       // NOTE 4C for 2^4
    0x8D,       // NOTE 3G for 2^4
    0x92,       // NOTE 4C for 2^4
    0x8A,       // NOTE 3E for 2^4
    0x92,       // NOTE 4C for 2^4
    0x8F,       // NOTE 3A for 2^4
    0x94,       // NOTE 4D for 2^4
    0x88,       // NOTE 3D for 2^4
    0x94,       // NOTE 4D for 2^4
    0x8F,       // NOTE 3A for 2^4
    0x94,       // NOTE 4D for 2^4
    0x88,       // NOTE 3D for 2^4
    0x94,       // NOTE 4D for 2^4
    0x8D,       // NOTE 3G for 2^4
    0x91,       // NOTE 3B for 2^4
    0x88,       // NOTE 3D for 2^4
    0x91,       // NOTE 3B for 2^4
    0x8D,       // NOTE 3G for 2^4
    0x91,       // NOTE 3B for 2^4
    0x88,       // NOTE 3D for 2^4
    0x91,       // NOTE 3B for 2^4
    0x8D,       // NOTE 3G for 2^4
    0x91,       // NOTE 3B for 2^4
    0x89,       // NOTE 3D# for 2^4
    0x91,       // NOTE 3B for 2^4
    0x8A,       // NOTE 3E for 2^4
    0x92,       // NOTE 4C for 2^4
    0x8F,       // NOTE 3A for 2^4
    0x92,       // NOTE 4C for 2^4
    0x8F,       // NOTE 3A for 2^4
    0x94,       // NOTE 4D for 2^4
    0x88,       // NOTE 3D for 2^4
    0x94,       // NOTE 4D for 2^4
    0x8F,       // NOTE 3A for 2^4
    0x94,       // NOTE 4D for 2^4
    0x88,       // NOTE 3D for 2^4
    0x94,       // NOTE 4D for 2^4
    0x8D,       // NOTE 3G for 2^4
    0x91,       // NOTE 3B for 2^4
    0x88,       // NOTE 3D for 2^4
    0x91,       // NOTE 3B for 2^4
    0xAD,       // NOTE 3G for 2^5
    0xA0,       // REST for 2^5
    0xD2,       // NOTE 4C for 2^6
    0xC0,       // REST for 2^6
    0xD1,       // NOTE 3B for 2^6
    0xC0,       // REST for 2^6
    0xFF,       // END OF VOICE
};

static VOICE voice[3];

void play() {
    uint8_t channel;
    SONG_DATA dat;

    for (channel = 0; channel < 3; channel++) {
        if (voice[channel].ptr == NULL)
            continue;

        if (voice[channel].length != 0) {
            voice[channel].length--;
            if (voice[channel].volume != 0 && (voice[channel].length & 1) == 0) {
                voice[channel].volume--;
                ay3891_set_volume(channel, voice[channel].volume);
            }
            continue;
        }

        dat.value = voice[channel].ptr[voice[channel].idx++];

        if (dat.value == 0xFF) {
            ay3891_set_volume(channel, 0);
            voice[channel].ptr = NULL;
            continue;
        }
        if (dat.n == CMD) {
            // TODO
        }
        else if (dat.n == REST) {
            voice[channel].length = time_table[dat.c];
            voice[channel].volume = 0;
            ay3891_set_volume(channel, 0);
        }
        else {
            voice[channel].length = time_table[dat.c];
            voice[channel].volume = voice[channel].volume_reload;
            ay3891_set_freq(channel, note_table[dat.n + voice[channel].offset - 1]);
            ay3891_set_volume(channel, voice[channel].volume);
            ay3891_trigger_envelope();
        }
    }
}

void start_intro() {
    voice[0].ptr = intro_voice_a;
    voice[0].idx = 0;
    voice[0].offset = 22;
    voice[0].length = 0;
    voice[0].volume = 0;
    voice[0].volume_reload = 15;

    voice[1].ptr = intro_voice_b;
    voice[1].idx = 0;
    voice[1].offset = 10;
    voice[1].length = 0;
    voice[1].volume = 0;
    voice[1].volume_reload = 15;

    voice[2].ptr = intro_voice_c;
    voice[2].idx = 10;
    voice[2].offset = 0;
    voice[2].length = 0;
    voice[2].volume = 0;
    voice[2].volume_reload = 15;
}

void start_main_song() {
    voice[0].ptr = main_a;
    voice[0].idx = 0;
    voice[0].offset = 22;
    voice[0].length = 0;
    voice[0].volume = 0;
    voice[0].volume_reload = 10;

    voice[1].ptr = main_b;
    voice[1].idx = 0;
    voice[1].offset = 10;
    voice[1].length = 0;
    voice[1].volume = 0;
    voice[1].volume_reload = 10;

    voice[2].ptr = NULL;
    voice[2].idx = 17;
    voice[2].offset = 0;
    voice[2].length = 0;
    voice[2].volume = 0;
    voice[2].volume_reload = 0;
}

int is_playing() {
    if (voice[0].ptr != NULL)
        return -1;
    if (voice[1].ptr != NULL)
        return -1;
    if (voice[2].ptr != NULL)
        return -1;
    return 0;
}

void stop_sound() {
    voice[0].ptr = NULL;
    voice[1].ptr = NULL;
    voice[2].ptr = NULL;

    ay3891_set_volume(0, 0);
    ay3891_set_volume(1, 0);
    ay3891_set_volume(2, 0);
}

void initialize_sound() {
    ay3891_start(26, 27);
    ay3891_reset_registers();
    ay3891_enable_tone(-1, -1, -1);
    ay3891_enable_noise(0, 0, 0);
    ay3891_set_envelope(0, 8192);
}
