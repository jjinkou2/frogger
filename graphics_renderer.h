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

#ifndef GRAPHICS_RENDERER_H_
#define GRAPHICS_RENDERER_H_

#include <propeller.h>

#define HTILES                  28
#define VTILES                  32

#define TILEMAP_HSIZE           ((HRES / 8) + 2)
#define TILEMAP_VSIZE           ((VRES / 8) + 2)

#define TILEMAP_SIZE            (TILEMAP_HSIZE * TILEMAP_VSIZE)
#define TILEMAP_SIZE_BYTES      TILEMAP_SIZE * 2

typedef union {
    uint16_t value;
    struct {
        uint8_t tile;
        uint8_t palette;
    };
} TILEMAP;

typedef union {
    uint32_t value;
    struct {
        uint32_t x       :9;        // Horizontal position
        uint32_t y       :9;        // Vertical position
        uint32_t palette :6;        // Palette index (0-63)
        uint32_t tile    :6;        // Tile index (0-63)
        uint32_t hflip   :1;        // 1=Horitonal flip
        uint32_t vflip   :1;        // 1=Vertical flip
    };
} SPRITE_STATE;

typedef struct {
    TILEMAP    * tile_map;
    uint32_t   * tiles_def;
    uint32_t   * sprites_def;
    uint32_t   * palette_def;
    uint32_t     x;
    uint32_t     y      :9;
    uint32_t     y_lock :9;
    uint32_t     res1   :14;
    SPRITE_STATE sprites[64];
} RENDER_PARAMS;

void start_renderers(TILEMAP * tile_map, uint32_t * palette_def, uint32_t * tiles_def, uint32_t * sprites_def);

void start_doublebuffer(TILEMAP * tile_map);
void flip();

void clear();

TILEMAP * get_tilemap_address(uint8_t x, uint8_t y);

void set_locked_lines(int lines);
void set_background_origin(int x, int y);

void sprite_show(uint8_t idx, uint8_t tile, uint8_t palette, uint16_t x, uint16_t y, uint8_t vflip, uint8_t hflip);
void sprite_hide(uint8_t idx);
void sprite_move(uint8_t idx, uint16_t x, uint16_t y);

void sprite_set_palette(uint8_t idx, uint8_t palette);
void sprite_set_hflip(uint8_t idx, uint8_t flip);
void sprite_set_vflip(uint8_t idx, uint8_t flip);

void tile_fill(uint8_t x, uint8_t y, uint16_t tile, uint8_t palette, uint8_t width, uint8_t height);
void tile_copy(uint8_t x, uint8_t y, uint16_t * map, uint8_t width, uint8_t height);

#endif /* GRAPHICS_RENDERER_H_ */
