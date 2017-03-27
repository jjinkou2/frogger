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

#ifndef NES_GAMEPAD_CTRL_H_
#define NES_GAMEPAD_CTRL_H_

#include <propeller.h>

#define JOY_CLK       2
#define JOY_LCH       3
#define JOY_DATAOUT0  4
#define JOY_DATAOUT1  5

void read_gamepad();

int player1_right();
int player1_left();
int player1_up();
int player1_down();
int player1_select();
int player1_start();
int player1_b();
int player1_fire();
int player1_none();

int player2_right();
int player2_left();
int player2_up();
int player2_down();
int player2_select();
int player2_start();
int player2_b();
int player2_fire();
int player2_none();

#endif /* NES_GAMEPAD_CTRL_H_ */
