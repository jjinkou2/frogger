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

#include <propeller.h>

/* Palettes */
#define DEFAULT 0
#define FONT_WHITE 1
#define FONT_RED 2
#define FONT_GREEN 3
#define FONT_YELLOW 4
#define FONT_WHITE_ON_BLUE 5

extern uint32_t palette_def[];

/* Tiles */
#define FONT_SPACE 0
#define FONT_0 1
#define FONT_A 11
#define FONT_MINUS 37
#define FONT_COPY 38
#define TOP_SEPARATOR1 39
#define TOP_SEPARATOR2 40
#define BOTTOM_SEPARATOR1 41
#define BOTTOM_SEPARATOR2 42
#define HOME_TOPLEFT 43
#define HOME_TOPBOTTOM 44
#define HOME_TOPRIGHT 45
#define HOME_MIDDLE 46
#define HOME_LEFT 47
#define HOME_RIGHT 48
#define HOME_BOTTOMLEFT 49
#define HOME_BOTTOMRIGHT 50
#define WATER 51
#define TIMER 52
#define TITLE_F 56
#define TITLE_R 60
#define TITLE_O 64
#define TITLE_G 68
#define TITLE_E 72
#define LIFE 76
#define LEVEL 77
#define FROG_AT_HOME 78

extern uint32_t tiles_def[];

/* Sprites */
#define LANE1 0
#define LANE3 1
#define LANE5A 2
#define LANE5B 3
#define LANE2 4
#define LANE4 5
#define LOGA 6
#define LOGB 7
#define LOGC 8
#define FROG1V 9
#define FROG2V 10
#define FROG3V 11
#define FROG1H 12
#define FROG2H 13
#define FROG3H 14
#define TURTLE1 15
#define TURTLE2 16
#define TURTLE3 17
#define ROAD_DEATH 18
#define DEATH 21
#define WATER_DEATH 22

extern uint32_t sprites_def[];

/* Maps */
#define HOME_INDEX 0
#define HOME_WIDTH 28
#define HOME_HEIGHT 3
#define ROAD_SEP_INDEX 1
#define ROAD_SEP_WIDTH 28
#define ROAD_SEP_HEIGHT 2

extern uint16_t * get_tilemap(int index);

extern uint16_t home[];
extern uint16_t road_sep[];
