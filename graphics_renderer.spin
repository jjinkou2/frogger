{{
┌──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                           Propeller Game Engine Graphics Renderer (C) 2013 Marco Maccaferri                                  │
├──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│                                               TERMS OF USE: MIT License                                                      │
├──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation    │ 
│files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,    │
│modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software│
│is furnished to do so, subject to the following conditions:                                                                   │
│                                                                                                                              │
│The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.│
│                                                                                                                              │
│THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE          │
│WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR         │
│COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,   │
│ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                         │
└──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
}}

CON

  hres    = 256
  vres    = 192
  htiles  = hres/8
  vtiles  = vres/8
  
  H_TILES = 34
  V_TILES = 34

  MAP_SIZE_LONG = H_TILES * V_TILES / 2
  MAP_SIZE_WORD = H_TILES * V_TILES
  MAP_SIZE_BYTE = H_TILES * V_TILES * 2
  
  ROW_SIZE_LONG = H_TILES / 2
  ROW_SIZE_WORD = H_TILES
  ROW_SIZE_BYTE = H_TILES * 2
  
  SPRITE_FLIP_MASK     = %1_0_000000_000000_000000000_000000000
  SPRITE_MIRROR_MASK   = %0_1_000000_000000_000000000_000000000
  SPRITE_TILE_MASK     = %0_0_111111_000000_000000000_000000000
  SPRITE_TILE_SHIFT    = 24
  SPRITE_PALETTE_MASK  = %0_0_000000_111111_000000000_000000000
  SPRITE_PALETTE_SHIFT = 18
  SPRITE_Y_MASK        = %0_0_000000_000000_111111111_000000000
  SPRITE_Y_SHIFT       = 9
  SPRITE_X_MASK        = %0_0_000000_000000_000000000_111111111
  SPRITE_X_SHIFT       = 0

  COGS = 4
  MAX_SPRITES = 64

  PARAM_COUNT = 6 + MAX_SPRITES

VAR

  long rend_tile_map
  long rend_tile_table
  long rend_sprite_table
  long rend_palette_table
  long rend_x_scroll
  long rend_y_scroll
  long rend_sprite_state[MAX_SPRITES]

  long offs_tile_map
  long offs_tile_table
  long offs_sprite_table
  long offs_palette_table
  long offs_x_scroll
  long offs_y_scroll
  long offs_sprite_state[MAX_SPRITES]

  long tile_map_1
  long tile_map_2
  long sprite_state

PUB start(p_sbuf, p_blnk, p_tile_map, p_tile_table, p_palette_table, p_sprite_table)

  base := p_sbuf                       ' scanline buffer
  blnk := p_blnk                       ' frame indicator

  longfill(@rend_sprite_state, $FFFFFFFF, MAX_SPRITES)
  longfill(@offs_sprite_state, $FFFFFFFF, MAX_SPRITES)

  tile_map_1 := tile_map_2 := p_tile_map
  sprite_state := @rend_sprite_state

  rend_tile_map := tile_map_1
  rend_tile_table := p_tile_table
  rend_palette_table := p_palette_table
  rend_sprite_table := p_sprite_table
  rend_x_scroll := 0
  rend_y_scroll := 0

  offs_tile_map := tile_map_2
  offs_tile_table := p_tile_table
  offs_palette_table := p_palette_table
  offs_sprite_table := p_sprite_table
  offs_x_scroll := 0
  offs_y_scroll := 0

  repeat offset from 0 to COGS-1
    cognew(@renderer, @rend_tile_map)
    waitcnt(cnt + 10000)

PUB start_doublebuffer(p_tile_map)

  offs_tile_map := tile_map_2 := p_tile_map
  sprite_state := @offs_sprite_state

PUB flip

  repeat
  while long[blnk] == vres
  repeat
  while long[blnk] <> vres

  longmove(@rend_tile_map, @offs_tile_map, PARAM_COUNT)

  if offs_tile_map == tile_map_1
    longmove(tile_map_2, tile_map_1, MAP_SIZE_LONG)
    offs_tile_map := tile_map_2
  else
    longmove(tile_map_1, tile_map_2, MAP_SIZE_LONG)
    offs_tile_map := tile_map_1

PUB get_tilemap_address(sx, sy)

  return offs_tile_map + (sy * H_TILES + sx) * 2

PUB clear

  longfill(offs_tile_map, 0, MAP_SIZE_LONG)
  longfill(sprite_state, $FFFFFFFF, MAX_SPRITES)

PUB set_background_origin(ox,oy)
 
  offs_x_scroll := ox
  offs_y_scroll := (offs_y_scroll & $FFFFFE00) | oy

PUB set_locked_lines(yl)
 
  offs_y_scroll := (offs_y_scroll & $1FF) | (yl << 9)

PUB sprite_show(si, st, sp, sx, sy, fl, mr) | t

  t := (st << SPRITE_TILE_SHIFT) | (sp << SPRITE_PALETTE_SHIFT) | (sy << SPRITE_Y_SHIFT) | sx
  if fl
    t |= SPRITE_FLIP_MASK
  if mr
    t |= SPRITE_MIRROR_MASK
  LONG[sprite_state][si] := t

PUB sprite_hide(si)

  LONG[sprite_state][si] := $FFFFFFFF

PUB sprite_flip(si,f)

  if f
    LONG[sprite_state][si] |= SPRITE_FLIP_MASK
  else
    LONG[sprite_state][si] &= !SPRITE_FLIP_MASK
 
PUB sprite_mirror(si,f)

  if f
    LONG[sprite_state][si] |= SPRITE_MIRROR_MASK
  else
    LONG[sprite_state][si] &= !SPRITE_MIRROR_MASK

PUB sprite_move(si,sx,sy) | t

  t := LONG[sprite_state][si] & $FFFC0000
  t |= (sy << 9) | sx
  LONG[sprite_state][si] := t

PUB sprite_get_x(si) | t

  t := LONG[sprite_state][si] & $1FF
  return t

PUB sprite_get_y(si) | t

  t := (LONG[sprite_state][si] >> 9) & $1FF
  return t

PUB sprite_palette(si,sp) | t

  t := LONG[sprite_state][si] & !SPRITE_PALETTE_MASK
  t |= sp << SPRITE_PALETTE_SHIFT
  LONG[sprite_state][si] := t

DAT             org     0

renderer        rdlong  a, blnk
                cmp     a, #vres wz
        if_ne   jmp     #$-2                    ' waiting for last line to be fetched

:vsync
                mov     lcnt, offset
                mov     last_palette, #$1FF

                rdlong  a, blnk
                cmp     a, #0 wz
        if_ne   jmp     #$-2

                ' begin parameters and sprite state copy
                mov     param_end_ptr, par
                movd    :rd0, #param_end_ptr -1
                movd    :rd1, #param_end_ptr -2
                add     param_end_ptr, #PARAM_COUNT * 4 -1
                movi    param_end_ptr, #PARAM_COUNT - 2
:rd0            rdlong  0-0, param_end_ptr
                sub     :rd0, inc_dest_2
                sub     param_end_ptr, i2s7 wc
:rd1            rdlong  0-0, param_end_ptr
                sub     :rd1, inc_dest_2
        if_nc   djnz    param_end_ptr, #:rd0
                ' end parameters and sprite state copy

                movs    :scr2, y_scroll
                movs    :scr3, y_scroll
                shr     y_scroll, #9
                movs    :scr1, y_scroll
                movs    :scr4, y_scroll

                rdlong  a, blnk
                cmp     a, #0 wz
        if_ne   jmp     #$-2                    ' waiting for end of vertical blank

:loop           mov     last_tile, last_sprite

                mov     hub_map_ptr, tile_map
                mov     a, lcnt
:scr1           cmp     lcnt, #0  wc,wz
:scr2   if_ae   add     a, #0
                shr     a, #3                   ' 8 lines per tile
                shl     a, #6                   ' 32 tiles per row
                add     hub_map_ptr, a
                shr     a, #4                   ' +2 tiles per row, 2 bytes per tile
                add     hub_map_ptr, a

                mov     hub_tile_ptr, tile_table
                mov     a, lcnt                 ' line offset within tile
:scr3   if_ae   add     a, #0
                and     a, #$07                 ' 8 lines per tile
                shl     a, #2                   ' 4 bytes per line
                add     hub_tile_ptr, a

                mov     ecnt, #34
                movd    :scanline_store1,#sbuf
                movd    :scanline_store2,#sbuf+1

        if_b    jmp     #$+6                
                mov     a, x_scroll
                and     a, #$FC
                shl     a, #7
                sub     :scanline_store1,a
                sub     :scanline_store2,a
                
:l2             rdword  tile, hub_map_ptr          ' read tile
                cmp     tile, last_tile wz
        if_e    jmp     #:same_tile
                mov     last_tile, tile

                mov     temp, tile
                shr     temp, #8
                cmp     temp, last_palette wz
        if_ne   call    #pload

                and     tile, #$FF
                shl     tile, #5                  ' 32 bytes per tile
                add     tile, hub_tile_ptr        ' convert to pointer

                rdlong  pixels1, tile
                
                ' begin pixel rendering
                mov     colors1, #0
                mov     colors2, #0

                mov     count, #4
                mov     a, pixels1
                and     a, mask15
                add     a, p2inc
                movs    :mv1, a
                rol     colors2, #8
:mv1            or      colors2, 0-0
                shr     a, #16
                movs    :mv2, a
                rol     colors1, #8
:mv2            or      colors1, 0-0
                shr     pixels1, #4
                djnz    count, #$-11
                ' end pixel rendering

:same_tile
:scanline_store1
                mov     0-0,colors1    'dest starts at cog_scanline_buffer
                add     :scanline_store1,inc_dest_2 'self-modify dest+2

:scanline_store2
                mov     0-0,colors2    'dest starts at cog_scanline_buffer
                add     :scanline_store2,inc_dest_2 'self-modify dest+2

                add     hub_map_ptr, #2
                djnz    ecnt, #:l2

                ' begin horizontal scroll
:scr4           cmp     lcnt, #0  wc,wz
        if_b    jmp     #:no_scroll
                
                mov     b, x_scroll
                and     b, #$03 wz
        if_z    jmp     #:no_scroll         

                movs    :src1h,#sbuf
                movs    :src2h,#sbuf+1
                movd    :dst1h,#sbuf

                shl     b, #3
                mov     x, #32
                sub     x, b
                cmp     x, #32 wz

                mov     count, #64

:src1h          mov     colors1, 0-0
:src2h          mov     colors2, 0-0

                shr     colors1, b
        if_nz   shl     colors2, x
        if_nz   or      colors1, colors2

:dst1h          mov     0-0, colors1

                add     :src1h, #1
                add     :src2h, #1
                add     :dst1h, inc_dest

                djnz    count, #:src1h
                ' end horizontal scroll

                ' begin sprite rendering
:no_scroll      movs    :next_sprite, #sprites
                mov     scnt, #MAX_SPRITES

:next_sprite    mov     tile, 0-0
                cmp     tile, last_sprite wz
        if_e    jmp     #:skip

                mov     y, tile
                shr     y, #9
                and     y, #$1FF

                mov     a, lcnt
                add     a, #16
                subs    a, y  wc,wz
        if_b    jmp     #:skip
                cmp     a,#16 wc,wz
        if_ae   jmp     #:skip

                test    tile, flip_mask wz      ' adjust y if sprite is flipped
        if_nz   mov     b, #15
        if_nz   sub     b, a
        if_nz   mov     a, b

                mov     hub_source, sprite_table
                and     a, #$0F                 ' 16 lines per tile
                shl     a, #3                   ' 8 bytes per line
                add     hub_source, a

                movs    :src1,#sbuf-4
                movs    :src2,#sbuf-3
                mov     a, tile
                and     a, #$1F8                ' x-position mask
                shr     a, #2
                add     :src1, a
                add     :src2, a

                movd    :dst1,#sbuf-4
                movd    :dst2,#sbuf-3
                shl     a, #9
                add     :dst1, a
                add     :dst2, a

                mov     x, tile
                and     x, #$07                 ' x-position mask
                mov     ccnt, #8
                sub     ccnt, x
                shl     x, #2

                mov     tile_ptr, tile
                and     tile_ptr, tile_mask
                shr     tile_ptr, #17           ' 128 bytes per tile
                add     tile_ptr, hub_source    ' convert to pointer

                test    tile, mirror_mask wz    ' adjust pointer if sprite is mirrored
        if_nz   add     tile_ptr, #4

                mov     temp, tile              ' load palette
                shr     temp, #SPRITE_PALETTE_SHIFT
                and     temp, #$3F
                cmp     temp, last_palette wz
        if_ne   call    #pload

                mov     ecnt, #16
                mov     pixels2, #0

:sloop          rdlong  temp, tile_ptr

:src1           mov     colors1, 0-0
:src2           mov     colors2, 0-0

                mov     pixels1, temp
                mov     a, #32
                sub     a, x

                test    tile, mirror_mask wz
        if_nz   jmp     #:mirrored

                ' begin sprite pixel rendering
                shr     pixels1, x
                or      pixels1, pixels2

                mov     pixels2, temp
                shl     pixels2, a

                mov     count, #4
                mov     a, pixels1
                and     a, mask15
                add     a, p2inc
                movs    :st1, a
                rol     colors2, #8
                test    pixels1, mask1 wz
        if_nz   and     colors2, longmask
:st1    if_nz   or      colors2, 0-0
                shr     a, #16
                movs    :st2, a
                rol     colors1, #8
                test    pixels1, mask5 wz
        if_nz   and     colors1, longmask
:st2    if_nz   or      colors1, 0-0
                shr     pixels1, #4
                djnz    count, #$-15
                ' end sprite pixel rendering

                jmp     #:dst1

                ' begin mirrored sprite pixel rendering
:mirrored       shl     pixels1, x
                or      pixels1, pixels2

                mov     pixels2, temp
                shr     pixels2, a

                mov     count, #4
                rol     pixels1, #4
                mov     a, pixels1
                and     a, mask15
                add     a, p2inc
                movs    :st3, a
                rol     colors2, #8
                test    pixels1, mask1 wz
        if_nz   and     colors2, longmask
:st3    if_nz   or      colors2, 0-0
                shr     a, #16
                movs    :st4, a
                rol     colors1, #8
                test    pixels1, mask5 wz
        if_nz   and     colors1, longmask
:st4    if_nz   or      colors1, 0-0
                djnz    count, #$-15
                ' end mirrored sprite pixel rendering

:dst1           mov     0-0, colors1
:dst2           mov     0-0, colors2

                add     :src1, #2
                add     :src2, #2
                add     :dst1, inc_dest_2
                add     :dst2, inc_dest_2

                test    tile, mirror_mask wz
        if_z    add     tile_ptr, #4
        if_nz   sub     tile_ptr, #4

                cmp     x, #0 wz
        if_z    mov     pixels2, #0
                mov     temp, #0

                sub     ecnt, ccnt  wc,wz
                mov     ccnt, #8
        if_be   jmp     #:skip
                cmp     ecnt, #8  wc,wz
        if_ae   jmp     #:sloop
                jmp     #:sloop+1

:skip           add     :next_sprite, #1
                djnz    scnt, #:next_sprite
                ' end sprite rendering
                
                mov     a, #32
                sub     a, #htiles wz
        if_z    jmp     #:w1                    ' check if we need a border
                
                movd    :blank1,#sbuf
                movd    :blank2,#sbuf+63
:blank1         mov     0-0, #0
:blank2         mov     0-0, #0
                add     :blank1, inc_dest
                sub     :blank2, inc_dest
                djnz    a, #:blank1

:w1             rdlong  a, blnk
                cmp     a, lcnt wz
        if_ne   jmp     #$-2                    ' waiting for line to be fetched

                ' begin scanline buffer copy
                mov     sbuf_ptr, base
                movd    :wr0, #sbuf_ptr -1      ' last long in cog buffer
                movd    :wr1, #sbuf_ptr -2      ' second-to-last long in cog buffer
                add     sbuf_ptr, #64 * 4 -1    ' last byte in hub buffer (8n + 7)
                movi    sbuf_ptr, #64 - 2       ' add magic marker
:wr0            wrlong  0-0, sbuf_ptr           ' |
                sub     :wr0, inc_dest_2        ' |
                sub     sbuf_ptr, i2s7 wc       ' |
:wr1            wrlong  0-0, sbuf_ptr           ' |
                sub     :wr1, inc_dest_2        ' |
        if_nc   djnz    sbuf_ptr, #:wr0         ' sub #7/djnz (Thanks Phil!)
                ' end scanline buffer copy

                add     lcnt, #COGS
                cmp     lcnt, #vres  wc,wz
        if_b    jmp     #:loop

                jmp     #:vsync

                ' begin palette load
pload           mov     last_palette, temp
                shl     temp, #4                  ' 16 bytes per palette
                add     temp, palette_table

                rdlong  colors1, temp
                movd    :pstore1, #palette
                add     temp, #4
                rdlong  colors2, temp
                movd    :pstore2, #palette+4
                add     temp, #4
                rdlong  colors3, temp
                movd    :pstore3, #palette+8
                add     temp, #4
                rdlong  colors4, temp
                movd    :pstore4, #palette+12

                mov     count, #4

:pl             mov     a, colors1
                and     a, #$FF
:pstore1        mov     0-0, a
                shr     colors1, #8
                add     :pstore1, inc_dest

                mov     a, colors2
                and     a, #$FF
:pstore2        mov     0-0, a
                shr     colors2, #8
                add     :pstore2, inc_dest

                mov     a, colors3
                and     a, #$FF
:pstore3        mov     0-0, a
                shr     colors3, #8
                add     :pstore3, inc_dest

                mov     a, colors4
                and     a, #$FF
:pstore4        mov     0-0, a
                shr     colors4, #8
                add     :pstore4, inc_dest

                djnz    count, #:pl

pload_ret       ret
                ' end palette load

' initialised data and/or presets

blnk            long    -4
base            long    +0
offset          long    0

mask1           long    $0_0_0_0_0_0_0_F
mask5           long    $0_0_0_F_0_0_0_0
mask15          long    $0_0_0_F_0_0_0_F

longmask        long    $FF_FF_FF_00
tile_mask       long    SPRITE_TILE_MASK
flip_mask       long    SPRITE_FLIP_MASK
mirror_mask     long    SPRITE_MIRROR_MASK

inc_dest        long    1 << 9 << 0
inc_dest_2      long    1 << 9 << 1
i2s7            long    2 << 23 | 7
p2inc           long    palette << 16 | palette

last_sprite     long    $FFFFFFFF

' uninitialised data and/or temporaries

a               res     1
b               res     1
x               res     1
y               res     1
tile            res     1
tile_ptr        res     1
last_tile       res     1
last_palette    res     1
palette         res     16                      ' palette buffer
pixels1         res     1
pixels2         res     1
temp            res     1
colors1         res     1
colors2         res     1
colors3         res     1
colors4         res     1
count           res     1

ecnt            res     1                       ' element count
lcnt            res     1                       ' line counter
scnt            res     1                       ' scanlines
ccnt            res     1

hub_map_ptr     res     1
hub_tile_ptr    res     1
hub_sprite_ptr  res     1
hub_source      res     1

' parameters

tile_map        res     1
tile_table      res     1
sprite_table    res     1
palette_table   res     1
x_scroll        res     1
y_scroll        res     1
sprites         res     MAX_SPRITES             ' sprites state buffer
param_end_ptr   res     1

                res     4
sbuf            res     64                      ' scanline buffer
sbuf_ptr        res     4

                fit     $1F0
