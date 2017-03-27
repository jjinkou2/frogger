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

#include "graphics_renderer.h"
#include "vga_driver.h"

#include "types.h"

#define RENDER_HTILES   0x00E8
#define RENDER_VRES     0x0100
#define RENDER_BLNK     0x0128
#define RENDER_BASE     0x0129
#define RENDER_OFFSET   0x012A

extern uint32_t binary_graphics_renderer_dat_start[];

static RENDER_PARAMS render_param;
static RENDER_PARAMS offscreen_param;

static TILEMAP      * tile_map_1;
static TILEMAP      * tile_map_2;
static SPRITE_STATE * sprites;

void start_renderers(TILEMAP * tile_map, uint32_t * palette_def, uint32_t * tiles_def, uint32_t * sprites_def) {
    int i;
    COGLONG * driver = (COGLONG *) binary_graphics_renderer_dat_start;

    driver[RENDER_VRES].src = VRES;
    driver[RENDER_HTILES].src = HTILES;

    memset(tile_map, 0x00, 34 * (VRES / 8 + 2) * 2);

    tile_map_1 = tile_map_2 = tile_map;
    sprites = render_param.sprites;

    render_param.tile_map = tile_map;
    render_param.tiles_def = tiles_def;
    render_param.sprites_def = sprites_def;
    render_param.palette_def = palette_def;
    render_param.x = 0;
    render_param.y = 0;
    memset(&render_param.sprites, 0xFF, sizeof(render_param.sprites));

    memcpy(&offscreen_param, &render_param, sizeof(render_param));

    driver[RENDER_BLNK].value = (uint32_t) &line_indicator;
    driver[RENDER_BASE].value = (uint32_t) &scanline_buffer;
    for (i = 0; i < 5; i++) {
        driver[RENDER_OFFSET].value = i;
        cognew(driver, &render_param);
        waitcnt(CNT + 10000);
    }
}

void start_doublebuffer(TILEMAP * tile_map) {
    offscreen_param.tile_map = tile_map_2 = tile_map;
    sprites = offscreen_param.sprites;
}

void flip() {
    while (line_indicator == VRES)
        ;
    while (line_indicator != VRES)
        ;

    memcpy(&render_param, &offscreen_param, sizeof(render_param));

    if (offscreen_param.tile_map == tile_map_1) {
        memcpy(tile_map_2, tile_map_1, TILEMAP_SIZE_BYTES);
        offscreen_param.tile_map = tile_map_2;
    }
    else {
        memcpy(tile_map_1, tile_map_2, TILEMAP_SIZE_BYTES);
        offscreen_param.tile_map = tile_map_1;
    }
}

void clear() {
    memset(offscreen_param.tile_map, 0x00, 34 * (VRES / 8 + 2) * 2);
    memset(sprites, 0xFF, sizeof(render_param.sprites));
}

TILEMAP * get_tilemap_address(uint8_t x, uint8_t y) {
    return &offscreen_param.tile_map[y * 34 + x];
}

void set_locked_lines(int lines) {
    offscreen_param.y_lock = lines;
}

void set_background_origin(int x, int y) {
    offscreen_param.x = x;
    offscreen_param.y = y;
}

void sprite_show(uint8_t idx, uint8_t tile, uint8_t palette, uint16_t x, uint16_t y, uint8_t vflip, uint8_t hflip) {
    sprites[idx].tile = tile;
    sprites[idx].palette = palette;
    sprites[idx].x = x;
    sprites[idx].y = y;
    sprites[idx].vflip = vflip;
    sprites[idx].hflip = hflip;
}

void sprite_hide(uint8_t idx) {
    sprites[idx].value = 0xFFFFFFFF;
}

void sprite_move(uint8_t idx, uint16_t x, uint16_t y) {
    sprites[idx].x = x;
    sprites[idx].y = y;
}

void sprite_set_hflip(uint8_t idx, uint8_t flip) {
    sprites[idx].hflip = flip;
}

void sprite_set_vflip(uint8_t idx, uint8_t flip) {
    sprites[idx].vflip = flip;
}

void sprite_set_palette(uint8_t idx, uint8_t palette) {
    sprites[idx].palette = palette;
}

void tile_fill(uint8_t x, uint8_t y, uint16_t tile, uint8_t palette, uint8_t width, uint8_t height) {
    int i;
    TILEMAP * map;

    tile |= palette << 8;

    while (height-- > 0) {
        map = &offscreen_param.tile_map[y * 34 + x];
        for (i = 0; i < width; i++) {
            map->value = tile;
            map++;
        }
        y++;
    }
}

void tile_copy(uint8_t x, uint8_t y, uint16_t * map, uint8_t width, uint8_t height) {
    TILEMAP * dst = &offscreen_param.tile_map[y * 34 + x];

    while (height-- > 0) {
        memcpy(dst, map, width * 2);
        dst += 34;
        map += width;
        y++;
    }
}
