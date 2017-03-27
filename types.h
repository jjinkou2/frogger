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
 *     Marco Maccaferri - adapted for game engine usage
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>

#define TRUE        -1
#define FALSE        0

typedef union {
    uint32_t value;
    struct {
        uint32_t src  :9;
        uint32_t dst  :9;
        uint32_t cond :4;
        uint32_t imm  :1;
        uint32_t wr   :1;
        uint32_t wc   :1;
        uint32_t wz   :1;
        uint32_t ins  :5;
    };
} COGLONG;

#endif /* TYPES_H_ */
