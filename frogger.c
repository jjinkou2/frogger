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

#include <string.h>
#include <stdint.h>
#include <propeller.h>

#include "vga_driver.h"
#include "graphics_renderer.h"
#include "nes_gamepad_ctrl.h"

#include "graphics.h"

extern uint32_t palette_def[];
extern uint32_t tiles_def[];
extern uint32_t sprites_def[];

extern void initialize_sound();
extern int is_playing();
extern void start_intro();
extern void start_main_song();
extern void play();
extern void stop_sound();

typedef struct {
    uint16_t tick;
    int16_t  increment;
    uint8_t  sprite;
    uint8_t  x;
    uint8_t  y;
    uint8_t  mirror;
} OBJECT_STATUS;

TILEMAP tile_map[TILEMAP_SIZE];

uint16_t objects;
OBJECT_STATUS object_status[64];

uint32_t play_counter;
uint32_t play_state;

uint16_t frog_object;
uint16_t frog_x;
uint16_t frog_y;
uint16_t frog_delay;
uint16_t frog_home[5];
uint16_t frog_highest_position;

uint16_t lifes;
uint16_t level;
uint16_t time_left;
uint16_t time_left_counter;
uint16_t notify_delay;

uint32_t score;
uint32_t highest_score;

int8_t place_log_sprite(int16_t x, int16_t y, int16_t span, int16_t tick, int16_t increment) {
    int16_t i;

    object_status[objects].tick = tick;
    object_status[objects].increment = increment;
    object_status[objects].sprite = LOGA;
    object_status[objects].x = x;
    object_status[objects].y = y;
    object_status[objects].mirror = 0;

    if (object_status[objects].x >= 0 && object_status[objects].x <= 240)
        sprite_show(objects, object_status[objects].sprite, 0, object_status[objects].x + 16, object_status[objects].y + 16, 0, 0);

    x += 16;
    objects++;

    for (i = 1; i < span - 1; i++) {
        object_status[objects].tick = tick;
        object_status[objects].increment = increment;
        object_status[objects].sprite = LOGB;
        object_status[objects].x = x;
        object_status[objects].y = y;
        object_status[objects].mirror = 0;

        if (object_status[objects].x >= 0 && object_status[objects].x <= 240)
            sprite_show(objects, object_status[objects].sprite, 0, object_status[objects].x + 16, object_status[objects].y + 16, 0, 0);

        x += 16;
        objects++;
    }

    if (span > 1) {
        object_status[objects].tick = tick;
        object_status[objects].increment = increment;
        object_status[objects].sprite = LOGC;
        object_status[objects].x = x;
        object_status[objects].y = y;
        object_status[objects].mirror = 0;

        if (object_status[objects].x >= 0 && object_status[objects].x <= 240)
            sprite_show(objects, object_status[objects].sprite, 0, object_status[objects].x + 16, object_status[objects].y + 16, 0, 0);

        x += 16;
        objects++;
    }

    return x;
}

int8_t place_sprite(int16_t x, int16_t y, int16_t sprite, int16_t span, int16_t tick, int16_t increment, int8_t mirror) {
    int16_t i;

    for (i = 0; i < span; i++) {
        object_status[objects].tick = tick;
        object_status[objects].increment = increment;
        object_status[objects].sprite = sprite;
        object_status[objects].x = x;
        object_status[objects].y = y;
        object_status[objects].mirror = mirror;

        if (object_status[objects].x >= 0 && object_status[objects].x <= 240)
            sprite_show(objects, object_status[objects].sprite, 0, object_status[objects].x + 16, object_status[objects].y + 16, 0, object_status[objects].mirror);

        x += 16;
        objects++;
    }

    return x;
}

void animate_sprites() {
    int16_t i, frog_capture, diff, water;
    OBJECT_STATUS * ptr;

    frog_capture = 0;
    water = 0;

    ptr = object_status;
    for (i = 0; i < objects; i++, ptr++) {
        if ((play_counter % ptr->tick) != 0)
            continue;

        ptr->x += ptr->increment;

        if ((play_counter % 15) == 0) {
            switch(ptr->sprite) {
                case TURTLE1:
                case TURTLE2:
                    ptr->sprite++;
                    break;
                case TURTLE3:
                    ptr->sprite = TURTLE1;
                    break;
            }
        }

        if (ptr->x >= 0 && ptr->x <= 240)
            sprite_show(i, ptr->sprite, 0, ptr->x + 16, ptr->y + 16, 0, ptr->mirror);
        else
            sprite_hide(i);

        if (!frog_capture && frog_y == ptr->y && play_state < 200) {
            if (frog_x > ptr->x) {
                diff = frog_x - ptr->x;
            }
            else {
                diff = ptr->x - frog_x;
            }
            if (ptr->y <= 112) {
                water = 1;
                if (diff <= 8) {
                    frog_x += ptr->increment;
                    if (frog_x <= 8 || frog_x > 232)
                        play_state = 201;
                    else
                        sprite_move(frog_object, frog_x + 16, frog_y + 16);
                    frog_capture = 1;
                }
            }
            else {
                if (diff < 12)
                    play_state = 201;
            }
        }
    }

    if (water && !frog_capture) {
        play_state = 205;
    }
}

void print_at(int x, int y, int palette, const char * s) {
    int c;
    TILEMAP * tile_map = get_tilemap_address(x, y);

    while (*s) {
        c = *s++;
        if (c >= '0' && c <= '9')
            c = (c - '0') + FONT_0;
        else if (c >= 'A' && c <= 'Z')
            c = (c - 'A') + FONT_A;
        else if (c == '-')
            c = FONT_MINUS;
        else if (c == 0x7F)
            c = FONT_COPY;
        else
            c = 0;
        tile_map->palette = palette;
        tile_map->tile = c;
        tile_map++;
    }
}

void print_dec_at(int x, int y, int palette, int32_t value) {
    int n, div = 1000000;
    TILEMAP * tile_map = get_tilemap_address(x, y);

    n = 0;
    while(div != 0) {
        if (value >= div) {
            tile_map->palette = palette;
            tile_map->tile = (value / div) + FONT_0;
            value %= div;
            tile_map++;
            n = 1;
        }
        else if (n || div == 1) {
            tile_map->palette = palette;
            tile_map->tile = FONT_0;
            tile_map++;
        }
        div /= 10;
    }
}

void print_score_at(int x, int y, int palette, int32_t value) {
    int i, n;
    TILEMAP * tile_map = get_tilemap_address(x, y);

    i = 6;
    do {
        n = value % 10;
        tile_map->palette = palette;
        tile_map->tile = n + FONT_0;
        tile_map--;
        value /= 10;
        i--;
    } while(value > 0);

    while(i-- > 0) {
        tile_map->palette = palette;
        tile_map->tile = FONT_SPACE;
        tile_map--;
    }
}

void update_score(int16_t increment) {
    score += increment;
    print_score_at(7, 1, FONT_RED, score);

    if (score > highest_score) {
        highest_score = score;
        print_score_at(16, 1, FONT_RED, highest_score);
    }
}

void initialize_objects() {
    int16_t x, y;

    objects = 0;
    y = 48;

    x = 16;
    x = place_log_sprite(x, y, 3, 2, 1) + 32;
    x = place_log_sprite(x, y, 3, 2, 1) + 32;
    x = place_log_sprite(x, y, 3, 2, 1);

    y += 16;

    x = 16;
    x = place_sprite(x, y, TURTLE1, 2, 3, -1, 0) + 32;
    x = place_sprite(x, y, TURTLE1, 2, 3, -1, 0) + 32;
    x = place_sprite(x, y, TURTLE1, 2, 3, -1, 0) + 32;
    x = place_sprite(x, y, TURTLE1, 2, 3, -1, 0);

    y += 16;

    x = 16;
    x = place_log_sprite(x, y, 5, 2, 1) + 48;
    x = place_log_sprite(x, y, 4, 2, 1);

    y += 16;

    x = 16;
    x = place_log_sprite(x, y, 3, 2, -1) + 32;
    x = place_log_sprite(x, y, 3, 2, -1) + 32;
    x = place_log_sprite(x, y, 3, 2, -1);

    y += 16;

    x = 16;
    x = place_sprite(x, y, TURTLE1, 3, 3, 1, 1) + 32;
    x = place_sprite(x, y, TURTLE1, 3, 3, 1, 1) + 32;
    x = place_sprite(x, y, TURTLE1, 3, 3, 1, 1);

    // 44 sprites used

    y += 32;

    x = 128;
    x = place_sprite(x, y, LANE5A, 1, 2, -1, 0);
    x = place_sprite(x, y, LANE5B, 1, 2, -1, 0) + 48;
    x = place_sprite(x, y, LANE5A, 1, 2, -1, 0);
    x = place_sprite(x, y, LANE5B, 1, 2, -1, 0);

    y += 16;

    x = 240;
    x = place_sprite(x, y, LANE4, 1, 2, 1, 0) + 48;

    y += 16;

    x = 128;
    x = place_sprite(x, y, LANE3, 1, 3, -1, 0) + 48;
    x = place_sprite(x, y, LANE3, 1, 3, -1, 0) + 48;
    x = place_sprite(x, y, LANE3, 1, 3, -1, 0);

    y += 16;

    x = 128;
    x = place_sprite(x, y, LANE2, 1, 4, 1, 0) + 48;
    x = place_sprite(x, y, LANE2, 1, 4, 1, 0);

    y += 16;

    x = 112;
    x = place_sprite(x, y, LANE1, 1, 4, -1, 0) + 48;
    x = place_sprite(x, y, LANE1, 1, 4, -1, 0) + 48;
    x = place_sprite(x, y, LANE1, 1, 4, -1, 0);

    // 57 sprites used

    frog_object = objects;

    play_counter = 0;
}

void setup_playfield() {
    int16_t i;

    clear();

    print_at(4, 0, FONT_WHITE, "1-UP");
    print_score_at(7, 1, FONT_RED, score);

    print_at(11, 0, FONT_WHITE, "HI-SCORE");
    print_score_at(16, 1, FONT_RED, highest_score);

    tile_copy(2, 3, home, HOME_WIDTH, HOME_HEIGHT);
    tile_fill(2, 6, WATER, DEFAULT, 28, 10);
    tile_copy(2, 16, road_sep, ROAD_SEP_WIDTH, ROAD_SEP_HEIGHT);
    tile_copy(2, 28, road_sep, ROAD_SEP_WIDTH, ROAD_SEP_HEIGHT);

    for (i = 0; i < objects; i++) {
        if (object_status[i].x >= 0 && object_status[i].x <= 240)
            sprite_show(i, object_status[i].sprite, 0, object_status[i].x + 16, object_status[i].y + 16, 0, object_status[i].mirror);
    }

    for (i = 0; i < 5; i++)
        frog_home[i] = 0;
}

void update_time() {
    uint8_t palette = FONT_GREEN;
    int32_t fulls = time_left / 4;
    int32_t fraction = time_left % 4;
    TILEMAP * tile_map = get_tilemap_address(25, 31);

    if (time_left <= 10) {
        palette = FONT_RED;
        tile_map[1].palette = palette;
        tile_map[2].palette = palette;
        tile_map[3].palette = palette;
        tile_map[4].palette = palette;
    }
    else {
        tile_map[1].palette = FONT_YELLOW;
        tile_map[2].palette = FONT_YELLOW;
        tile_map[3].palette = FONT_YELLOW;
        tile_map[4].palette = FONT_YELLOW;
    }

    while(fulls-- > 0) {
        tile_map->palette = palette;
        tile_map->tile = TIMER;
        tile_map--;
    }

    tile_map->palette = palette;
    if (fraction == 0)
        tile_map->tile = FONT_SPACE;
    else
        tile_map->tile = TIMER + (4 - fraction);

    if (time_left <= 0) {
        print_at(11, 17, FONT_RED, "TIME OVER");
        notify_delay = 120;
        play_state = 201;
    }
}

void update_lifes() {
    int16_t i;
    TILEMAP * tile_map = get_tilemap_address(2, 30);

    for (i = 1; i <= 10; i++) {
        tile_map->tile = ((lifes - 1) >= i) ? LIFE : FONT_SPACE;
        tile_map++;
    }
}

void update_level() {
    int16_t i;
    TILEMAP * tile_map = get_tilemap_address(25, 30);

    for (i = 1; i <= 10; i++) {
        tile_map->tile = (level >= i) ? LEVEL : FONT_SPACE;
        tile_map++;
    }
}

void clear_home() {
    int16_t i, tx;

    tx = 3;

    for (i = 0; i < 5; i++) {
        get_tilemap_address(tx, 4)->tile = WATER;
        get_tilemap_address(tx + 1, 4)->tile = WATER;
        get_tilemap_address(tx, 5)->tile = WATER;
        get_tilemap_address(tx + 1, 5)->tile = WATER;
        frog_home[i] = 0;
        tx += 6;
    }
}

int32_t is_level_complete() {
    int16_t i;

    for (i = 0; i < 5; i++) {
        if (!frog_home[i])
            return 0;
    }

    return 1;
}

void check_home_jump() {
    int16_t i, hx, tx, idx;

    hx = 24;
    tx = 3;
    idx = 0;

    for (i = 0; i < 5; i++) {
        if (frog_x >= (hx - 4) && frog_x < (hx + 4)) {
            if (frog_y == 32) {
                get_tilemap_address(tx, 4)->tile = FROG_AT_HOME;
                get_tilemap_address(tx + 1, 4)->tile = FROG_AT_HOME + 1;
                get_tilemap_address(tx, 5)->tile = FROG_AT_HOME + 2;
                get_tilemap_address(tx + 1, 5)->tile = FROG_AT_HOME + 3;

                frog_home[idx] = 1;

                print_at(12, 17, FONT_RED, "TIME ");
                print_dec_at(17, 17, FONT_RED, 60 - time_left);
                notify_delay = 60 * 5;

                update_score(50 + time_left * 10);

                if (is_level_complete()) {
                    sprite_hide(frog_object);
                    play_state = 302;
                }
                else {
                    frog_x = 120;
                    frog_y = 224;
                    frog_highest_position = frog_y;
                    sprite_show(frog_object, FROG1V, 0, frog_x + 16, frog_y + 16, 0, 0);

                    time_left = 60;
                    time_left_counter = 0;
                    update_time();

                    play_state = 99;
                }

                return;
            }

            if (frog_y == 40) {
                if (frog_home[idx])
                    play_state = 201;
                return;
            }
        }

        hx += 6 * 8;
        tx += 6;
        idx++;
    }

    play_state = 201;
}

void game() {
    score = 0;
    lifes = 3;
    level = 1;

    clear();
    flip();

    setup_playfield();
    print_at(26, 31, FONT_YELLOW, "TIME");
    update_lifes();
    update_level();

    frog_x = 120;
    frog_y = 224;
    frog_highest_position = frog_y;
    sprite_show(frog_object, FROG1V, 0, frog_x + 16, frog_y + 16, 0, 0);

    play_state = 0;
    notify_delay = 0;

    time_left = 60;
    time_left_counter = 0;
    update_time();

    start_intro();

    for (;;) {
        flip();
        play_counter++;

        read_gamepad();

        if (play_state < 200) {
            if (!is_playing()) {
                start_main_song();
            }
            play();
        }

        if (notify_delay != 0) {
            if (--notify_delay == 0) {
                tile_copy(2, 16, road_sep, ROAD_SEP_WIDTH, ROAD_SEP_HEIGHT);
            }
        }

        switch(play_state) {
            case 0:
                if (player1_up()) {
                    if (frog_y > 32) {
                        frog_y -= 8;
                        sprite_show(frog_object, FROG3V, 0, frog_x + 16, frog_y + 16, 0, 0);
                        frog_delay = 4;
                        play_state = 1;
                    }
                }
                else if (player1_down()) {
                    if (frog_y < 224) {
                        frog_y += 8;
                        sprite_show(frog_object, FROG3V, 0, frog_x + 16, frog_y + 16, 1, 1);
                        frog_delay = 4;
                        play_state = 2;
                    }
                }
                else if (player1_right()) {
                    if (frog_x < 224) {
                        frog_x += 8;
                        sprite_show(frog_object, FROG3H, 0, frog_x + 16, frog_y + 16, 1, 1);
                        frog_delay = 4;
                        play_state = 3;
                    }
                }
                else if (player1_left()) {
                    if (frog_x > 16) {
                        frog_x -= 8;
                        sprite_show(frog_object, FROG3H, 0, frog_x + 16, frog_y + 16, 0, 0);
                        frog_delay = 4;
                        play_state = 4;
                    }
                }
                break;
            case 1:
                if (--frog_delay == 0) {
                    if (frog_y == 40 || frog_y == 32)
                      check_home_jump();

                    if (play_state == 1) {
                        frog_y -= 8;
                        if (frog_y == 40 || frog_y == 32)
                          check_home_jump();
                    }

                    if (play_state == 1) {
                        sprite_show(frog_object, FROG1V, 0, frog_x + 16, frog_y + 16, 0, 0);
                        if (frog_y < frog_highest_position) {
                            if ((frog_y >= 48 && frog_y < 128) || (frog_y >= 144 && frog_y < 224))
                                update_score(10);
                            frog_highest_position = frog_y;
                        }
                        play_state = 99;
                    }
                }
                break;
            case 2:
                if (--frog_delay == 0) {
                    frog_y += 8;
                    sprite_show(frog_object, FROG1V, 0, frog_x + 16, frog_y + 16, 1, 1);
                    play_state = 99;
                }
                break;
            case 3:
                if (--frog_delay == 0) {
                    frog_x += 8;
                    sprite_show(frog_object, FROG1H, 0, frog_x + 16, frog_y + 16, 1, 1);
                    play_state = 99;
                }
                break;
            case 4:
                if (--frog_delay == 0) {
                    frog_x -= 8;
                    sprite_show(frog_object, FROG1H, 0, frog_x + 16, frog_y + 16, 0, 0);
                    play_state = 99;
                }
                break;
            case 99:
                if (player1_none()) {
                    play_state = 0;
                }
                break;

            case 201: // Death in road
                stop_sound();
                sprite_show(frog_object, ROAD_DEATH, 0, frog_x + 16, frog_y + 16, 0, 0);
                frog_delay = 25;
                play_state++;
                break;
            case 202:
                if (--frog_delay == 0) {
                    sprite_show(frog_object, ROAD_DEATH + 1, 0, frog_x + 16, frog_y + 16, 0, 0);
                    frog_delay = 25;
                    play_state++;
                }
                break;
            case 203:
                if (--frog_delay == 0) {
                    sprite_show(frog_object, ROAD_DEATH + 2, 0, frog_x + 16, frog_y + 16, 0, 0);
                    frog_delay = 25;
                    play_state++;
                }
                break;
            case 204:
                if (--frog_delay == 0) {
                    sprite_show(frog_object, DEATH, 0, frog_x + 16, frog_y + 16, 0, 0);
                    frog_delay = 35;
                    play_state = 300;
                }
                break;

            case 205: // Death in water
                stop_sound();
                sprite_show(frog_object, WATER_DEATH, 0, frog_x + 16, frog_y + 16, 0, 0);
                frog_delay = 25;
                play_state++;
                break;
            case 206:
                if (--frog_delay == 0) {
                    sprite_show(frog_object, WATER_DEATH + 1, 0, frog_x + 16, frog_y + 16, 0, 0);
                    frog_delay = 25;
                    play_state++;
                }
                break;
            case 207:
                if (--frog_delay == 0) {
                    sprite_show(frog_object, WATER_DEATH + 2, 0, frog_x + 16, frog_y + 16, 0, 0);
                    frog_delay = 25;
                    play_state++;
                }
                break;
            case 208:
                if (--frog_delay == 0) {
                    sprite_show(frog_object, DEATH, 0, frog_x + 16, frog_y + 16, 0, 0);
                    frog_delay = 35;
                    play_state = 300;
                }
                break;

            case 300: // Next life
                if (--frog_delay == 0) {
                    lifes--;
                    update_lifes();
                    if (lifes == 0) {
                        print_at(11, 17, FONT_RED, "GAME OVER");
                        notify_delay = 0;
                        sprite_hide(frog_object);
                        play_state = 301;
                        frog_delay = 60 * 10;
                        break;
                    }

                    frog_x = 120;
                    frog_y = 224;
                    sprite_show(frog_object, FROG1V, 0, frog_x + 16, frog_y + 16, 0, 0);

                    time_left = 60;
                    time_left_counter = 0;
                    update_time();

                    play_state = 0;
                }
                break;
            case 301:
                if (--frog_delay == 0) {
                    return;
                }
                break;

            case 302: // Next level
                start_intro();
                play_state++;
                break;
            case 303:
                play();
                if (!is_playing()) {
                    level++;
                    update_level();
                    clear_home();

                    frog_x = 120;
                    frog_y = 224;
                    sprite_show(frog_object, FROG1V, 0, frog_x + 16, frog_y + 16, 0, 0);

                    time_left = 60;
                    time_left_counter = 0;
                    update_time();

                    start_main_song();

                    play_state = 99;
                }
                break;
        }

        animate_sprites();

        if (play_state < 200) {
            time_left_counter++;
            if (time_left_counter >= 60) {
                time_left--;
                time_left_counter = 0;
                update_time();
            }
        }
    }
}

int32_t game_sample() {
    uint32_t timer, elapsed;

    clear();
    flip();

    setup_playfield();
    print_at(7, 17, FONT_RED, "PRESS START TO PLAY");

    timer = CNT;

    do {
        flip();
        play_counter++;

        read_gamepad();

        if (player1_start()) {
            return 1;
        }

        animate_sprites();

        elapsed = (CNT - timer) / CLKFREQ;
    } while(elapsed < 20);

    return 0;
}

int32_t intro() {
    int16_t anim_counter = 10;
    int16_t anim_x = 272;
    int16_t anim_y = 144;
    int16_t anim_frame = FROG1H;
    int16_t anim_state = 0;
    int16_t title_x = 3;
    int16_t title_y = 6;

    clear();
    flip();

    tile_fill(2, 2, WATER, DEFAULT, 28, 14);
    print_at(7, 22, FONT_YELLOW, "PRESS START TO PLAY");
    sprite_show(0, anim_frame, 0, anim_x, anim_y, 0, 0);

    for (;;) {
        flip();
        read_gamepad();

        if (player1_start()) {
            return 1;
        }

        if (--anim_counter != 0) {
            continue;
        }

        switch(anim_state) {
            case 0:
                anim_x -= 4;
                if (anim_x == 136) {
                    sprite_show(0, FROG1V, 0, anim_x, anim_y, 0, 0);
                    anim_counter = 60;
                    anim_state = 1;
                    break;
                }

                if (anim_frame == FROG1H)
                    anim_frame = FROG2H;
                else if (anim_frame == FROG2H)
                    anim_frame = FROG3H;
                else if (anim_frame == FROG3H)
                    anim_frame = FROG1H;

                sprite_show(0, anim_frame, 0, anim_x, anim_y, 0, 0);

                anim_counter = 10;
                break;
            case 1:
                get_tilemap_address(title_x, title_y)->tile = TITLE_F;
                get_tilemap_address(title_x + 1, title_y)->tile = TITLE_F + 1;
                get_tilemap_address(title_x, title_y + 1)->tile = TITLE_F + 2;
                get_tilemap_address(title_x + 1, title_y + 1)->tile = TITLE_F + 3;
                title_x += 4;
                anim_counter = 60;
                anim_state++;
                break;
            case 2:
                get_tilemap_address(title_x, title_y)->tile = TITLE_R;
                get_tilemap_address(title_x + 1, title_y)->tile = TITLE_R + 1;
                get_tilemap_address(title_x, title_y + 1)->tile = TITLE_R + 2;
                get_tilemap_address(title_x + 1, title_y + 1)->tile = TITLE_R + 3;
                title_x += 4;
                anim_counter = 60;
                anim_state++;
                break;
            case 3:
                get_tilemap_address(title_x, title_y)->tile = TITLE_O;
                get_tilemap_address(title_x + 1, title_y)->tile = TITLE_O + 1;
                get_tilemap_address(title_x, title_y + 1)->tile = TITLE_O + 2;
                get_tilemap_address(title_x + 1, title_y + 1)->tile = TITLE_O + 3;
                title_x += 4;
                anim_counter = 60;
                anim_state++;
                break;
            case 4:
                get_tilemap_address(title_x, title_y)->tile = TITLE_G;
                get_tilemap_address(title_x + 1, title_y)->tile = TITLE_G + 1;
                get_tilemap_address(title_x, title_y + 1)->tile = TITLE_G + 2;
                get_tilemap_address(title_x + 1, title_y + 1)->tile = TITLE_G + 3;
                title_x += 4;
                anim_counter = 60;
                anim_state++;
                break;
            case 5:
                get_tilemap_address(title_x, title_y)->tile = TITLE_G;
                get_tilemap_address(title_x + 1, title_y)->tile = TITLE_G + 1;
                get_tilemap_address(title_x, title_y + 1)->tile = TITLE_G + 2;
                get_tilemap_address(title_x + 1, title_y + 1)->tile = TITLE_G + 3;
                title_x += 4;
                anim_counter = 60;
                anim_state++;
                break;
            case 6:
                get_tilemap_address(title_x, title_y)->tile = TITLE_E;
                get_tilemap_address(title_x + 1, title_y)->tile = TITLE_E + 1;
                get_tilemap_address(title_x, title_y + 1)->tile = TITLE_E + 2;
                get_tilemap_address(title_x + 1, title_y + 1)->tile = TITLE_E + 3;
                title_x += 4;
                anim_counter = 60;
                anim_state++;
                break;
            case 7:
                get_tilemap_address(title_x, title_y)->tile = TITLE_R;
                get_tilemap_address(title_x + 1, title_y)->tile = TITLE_R + 1;
                get_tilemap_address(title_x, title_y + 1)->tile = TITLE_R + 2;
                get_tilemap_address(title_x + 1, title_y + 1)->tile = TITLE_R + 3;
                print_at(5, title_y + 4, FONT_WHITE_ON_BLUE, "PROPELLER GAME ENGINE");
                print_at(4, title_y + 6, FONT_WHITE_ON_BLUE, "DEMO BY MARCO MACCAFERRI");
                print_at(10, 30, FONT_WHITE, "KONAMI \x7F 1981");
                anim_counter = 60 * 10;
                anim_state++;
                break;
            case 8:
                return 0;
        }
    }
}

int main() {
    int32_t result;

    start_vga_driver();

    start_renderers(tile_map, palette_def, tiles_def, sprites_def);
    start_doublebuffer(tile_map);

    initialize_sound();

    highest_score = 0;

    initialize_objects();

    for (;;) {
        result = intro();
        result = 1; // temporary
        if (!result) {
            result = game_sample();
        }
        if (result) {
            game();
        }
    }

    return 0;
}
