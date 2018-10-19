 GNU assembler version 2.21 (propeller-elf)
	 using BFD version (propellergcc_v1_0_0_2408) 2.21.
 options passed	: -lmm -cmm -ahdlnsg=cmm/nes_gamepad_ctrl.asm 
 input file    	: C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s
 output file   	: cmm/nes_gamepad_ctrl.o
 target        	: propeller-parallax-elf
 time stamp    	: 

   1              		.text
   2              	.Ltext0
   3              		.global	_read_gamepad
   4              	_read_gamepad
   5              	.LFB0
   6              		.file 1 "nes_gamepad_ctrl.c"
   1:nes_gamepad_ctrl.c **** /*
   2:nes_gamepad_ctrl.c ****  * Propeller Game Engine (C) 2013 Marco Maccaferri
   3:nes_gamepad_ctrl.c ****  * Based on code written by Jeff Ledger / Oldbitcollector
   4:nes_gamepad_ctrl.c ****  *
   5:nes_gamepad_ctrl.c ****  * TERMS OF USE: MIT License
   6:nes_gamepad_ctrl.c ****  *
   7:nes_gamepad_ctrl.c ****  * Permission is hereby granted, free of charge, to any person obtaining a copy of this software an
   8:nes_gamepad_ctrl.c ****  * files (the "Software"), to deal in the Software without restriction, including without limitatio
   9:nes_gamepad_ctrl.c ****  * modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permi
  10:nes_gamepad_ctrl.c ****  * is furnished to do so, subject to the following conditions:
  11:nes_gamepad_ctrl.c ****  *
  12:nes_gamepad_ctrl.c ****  * The above copyright notice and this permission notice shall be included in all copies or substan
  13:nes_gamepad_ctrl.c ****  *
  14:nes_gamepad_ctrl.c ****  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BU
  15:nes_gamepad_ctrl.c ****  * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
  16:nes_gamepad_ctrl.c ****  * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF C
  17:nes_gamepad_ctrl.c ****  * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFT
  18:nes_gamepad_ctrl.c ****  *
  19:nes_gamepad_ctrl.c ****  * Contributors:
  20:nes_gamepad_ctrl.c ****  *     Jeff Ledger / Oldbitcollector - initial API and implementation
  21:nes_gamepad_ctrl.c ****  *     Marco Maccaferri - adapted for game engine usage
  22:nes_gamepad_ctrl.c ****  */
  23:nes_gamepad_ctrl.c **** 
  24:nes_gamepad_ctrl.c **** #include "nes_gamepad_ctrl.h"
  25:nes_gamepad_ctrl.c **** 
  26:nes_gamepad_ctrl.c **** #define NES0_RIGHT  0x0001
  27:nes_gamepad_ctrl.c **** #define NES0_LEFT   0x0002
  28:nes_gamepad_ctrl.c **** #define NES0_DOWN   0x0004
  29:nes_gamepad_ctrl.c **** #define NES0_UP     0x0008
  30:nes_gamepad_ctrl.c **** #define NES0_START  0x0010
  31:nes_gamepad_ctrl.c **** #define NES0_SELECT 0x0020
  32:nes_gamepad_ctrl.c **** #define NES0_B      0x0040
  33:nes_gamepad_ctrl.c **** #define NES0_A      0x0080
  34:nes_gamepad_ctrl.c **** 
  35:nes_gamepad_ctrl.c **** #define NES1_RIGHT  0x0100
  36:nes_gamepad_ctrl.c **** #define NES1_LEFT   0x0200
  37:nes_gamepad_ctrl.c **** #define NES1_DOWN   0x0400
  38:nes_gamepad_ctrl.c **** #define NES1_UP     0x0800
  39:nes_gamepad_ctrl.c **** #define NES1_START  0x1000
  40:nes_gamepad_ctrl.c **** #define NES1_SELECT 0x2000
  41:nes_gamepad_ctrl.c **** #define NES1_B      0x4000
  42:nes_gamepad_ctrl.c **** #define NES1_A      0x8000
  43:nes_gamepad_ctrl.c **** 
  44:nes_gamepad_ctrl.c **** static uint16_t nes_bits;
  45:nes_gamepad_ctrl.c **** 
  46:nes_gamepad_ctrl.c **** void read_gamepad() {
   7              		.loc 1 46 0
  47:nes_gamepad_ctrl.c ****     int i;
  48:nes_gamepad_ctrl.c **** 
  49:nes_gamepad_ctrl.c ****     // step 1: set I/Os
  50:nes_gamepad_ctrl.c ****     DIRA |= (1 << JOY_CLK) | (1 << JOY_LCH);
   8              		.loc 1 50 0
   9 0000 F2000EA0 		mov	r7, DIRA
  10              	.LVL0
  11 0004 27C7     		or	r7, #12
  12              	.LVL1
  13 0006 F20700A0 		mov	DIRA, r7
  14              	.LVL2
  51:nes_gamepad_ctrl.c **** 
  52:nes_gamepad_ctrl.c ****     // step 2: set clock and latch to 0
  53:nes_gamepad_ctrl.c ****     OUTA &= ~((1 << JOY_CLK) | (1 << JOY_LCH));
  15              		.loc 1 53 0
  16 000a F2000EA0 		mov	r7, OUTA
  17              	.LVL3
  18 000e 27C5     		andn	r7, #0xc
  19              	.LVL4
  20 0010 F20700A0 		mov	OUTA, r7
  21              	.LVL5
  54:nes_gamepad_ctrl.c **** 
  55:nes_gamepad_ctrl.c ****     // step 3: set latch to 1
  56:nes_gamepad_ctrl.c ****     OUTA |= (1 << JOY_LCH);
  22              		.loc 1 56 0
  23 0014 F2000EA0 		mov	r7, OUTA
  24 0018 2787     		or	r7, #8
  25              	.LVL6
  26 001a F20700A0 		mov	OUTA, r7
  27              	.LVL7
  57:nes_gamepad_ctrl.c **** 
  58:nes_gamepad_ctrl.c ****     // step 4: set latch to 0
  59:nes_gamepad_ctrl.c ****     OUTA &= ~(1 << JOY_LCH);
  28              		.loc 1 59 0
  29 001e F2000EA0 		mov	r7, OUTA
  30 0022 2785     		andn	r7, #0x8
  31              	.LVL8
  60:nes_gamepad_ctrl.c **** 
  61:nes_gamepad_ctrl.c ****     // data is now ready to shift out, clear storage
  62:nes_gamepad_ctrl.c ****     nes_bits = 0;
  32              		.loc 1 62 0
  33 0024 B6       		mov	r6, #0
  59:nes_gamepad_ctrl.c ****     OUTA &= ~(1 << JOY_LCH);
  34              		.loc 1 59 0
  35 0025 F20700A0 		mov	OUTA, r7
  36              	.LVL9
  37              		.loc 1 62 0
  38 0029 670000   		mviw	r7,#_nes_bits
  39              	.LVL10
  40 002c A408     		mov	r4, #8
  41 002e F0070C04 		wrword	r6, r7
  42              	.LVL11
  43 0032 B6       		mov	r6, #0
  44              	.LVL12
  45              	.L2
  63:nes_gamepad_ctrl.c **** 
  64:nes_gamepad_ctrl.c ****     // step 5: read 8 bits, 1st bits are already latched and ready, simply save and clock remaining
  65:nes_gamepad_ctrl.c ****     for (i = 0; i < 8; i++) {
  66:nes_gamepad_ctrl.c ****       nes_bits <<= 1;
  67:nes_gamepad_ctrl.c ****       nes_bits |= ((INA >> JOY_DATAOUT0) & 1) | (((INA >> JOY_DATAOUT1) & 1) << 8);
  46              		.loc 1 67 0 discriminator 2
  47 0033 F2000AA0 		mov	r5, INA
  48 0037 F2000EA0 		mov	r7, INA
  49              	.LVL13
  50 003b 2739     		shl	r7, #3
  51 003d 254A     		shr	r5, #4
  66:nes_gamepad_ctrl.c ****       nes_bits <<= 1;
  52              		.loc 1 66 0 discriminator 2
  53 003f 2619     		shl	r6, #1
  54              		.loc 1 67 0 discriminator 2
  55 0041 370041   		and	r7, #256
  56 0044 2514     		and	r5, #1
  57 0046 1757     		or	r7, r5
  66:nes_gamepad_ctrl.c ****       nes_bits <<= 1;
  58              		.loc 1 66 0 discriminator 2
  59 0048 F2000C60 		and	r6,__MASK_0000FFFF
  60              		.loc 1 67 0 discriminator 2
  61 004c 1677     		or	r6, r7
  68:nes_gamepad_ctrl.c **** 
  69:nes_gamepad_ctrl.c ****       OUTA |= (1 << JOY_CLK); // JOY_CLK = 1
  62              		.loc 1 69 0 discriminator 2
  63 004e F2000EA0 		mov	r7, OUTA
  64              	.LVL14
  65 0052 2747     		or	r7, #4
  66              	.LVL15
  67 0054 F20700A0 		mov	OUTA, r7
  68              	.LVL16
  70:nes_gamepad_ctrl.c ****       OUTA &= ~(1 << JOY_CLK); // JOY_CLK = 0
  69              		.loc 1 70 0 discriminator 2
  70 0058 F2000EA0 		mov	r7, OUTA
  71 005c 2745     		andn	r7, #0x4
  72              	.LVL17
  73 005e F20700A0 		mov	OUTA, r7
  74              	.LVL18
  65:nes_gamepad_ctrl.c ****     for (i = 0; i < 8; i++) {
  75              		.loc 1 65 0 discriminator 2
  76 0062 FB010884 		sub	r4, #1 wz
  77 0066 75CB     		IF_NE	brs	#.L2
  71:nes_gamepad_ctrl.c ****     }
  72:nes_gamepad_ctrl.c **** 
  73:nes_gamepad_ctrl.c ****     // invert bits to make positive logic
  74:nes_gamepad_ctrl.c ****     nes_bits = (~nes_bits & 0xFFFF);
  78              		.loc 1 74 0
  79 0068 36FF8F   		xor	r6,__MASK_FFFFFFFF
  80 006b F2000C60 		and	r6,__MASK_0000FFFF
  75:nes_gamepad_ctrl.c **** 
  76:nes_gamepad_ctrl.c ****     if ((nes_bits & 0xFF) == 0xFF)
  81              		.loc 1 76 0
  82 006f 0A56     		mov	r5, r6
  83 0071 35FF40   		and	r5, #255
  84 0074 35FF20   		cmps	r5, #255 wz,wc
  74:nes_gamepad_ctrl.c ****     nes_bits = (~nes_bits & 0xFFFF);
  85              		.loc 1 74 0
  86 0077 670000   		mviw	r7,#_nes_bits
  87              	.LVL19
  77:nes_gamepad_ctrl.c ****       nes_bits = nes_bits & 0xFF00;
  78:nes_gamepad_ctrl.c **** 
  79:nes_gamepad_ctrl.c ****     if ((nes_bits & 0xFF00) == 0xFF00)
  88              		.loc 1 79 0
  89 007a 6500FF   		mviw	r5,#65280
  74:nes_gamepad_ctrl.c ****     nes_bits = (~nes_bits & 0xFFFF);
  90              		.loc 1 74 0
  91 007d F0070C04 		wrword	r6, r7
  77:nes_gamepad_ctrl.c ****       nes_bits = nes_bits & 0xFF00;
  92              		.loc 1 77 0
  93 0081 9536FF50 		IF_E  andn	r6, #0xff
  94 0085 0F070C28 		IF_E  wrword	r6, r7
  94      04
  95              		.loc 1 79 0
  96 008a 660000   		mviw	r6,#_nes_bits
  97 008d F2060E04 		rdword	r7, r6
  98 0091 D44754   		xmov	r4,r7 and r4,r5
  99 0094 1452     		cmps	r4, r5 wz,wc
  80:nes_gamepad_ctrl.c ****       nes_bits = nes_bits & 0xFF;
 100              		.loc 1 80 0
 101 0096 9537FF40 		IF_E  and	r7, #255
 102 009a 0F060E28 		IF_E  wrword	r7, r6
 102      04
 103 009f 02       		lret
 104              	.LFE0
 105              		.global	_player1_right
 106              	_player1_right
 107              	.LFB1
  81:nes_gamepad_ctrl.c **** }
  82:nes_gamepad_ctrl.c **** 
  83:nes_gamepad_ctrl.c **** int player1_right() {
 108              		.loc 1 83 0
  84:nes_gamepad_ctrl.c ****     return (nes_bits & NES0_RIGHT) != 0;
 109              		.loc 1 84 0
 110 00a0 670000   		mviw	r7,#_nes_bits
 111 00a3 F2070004 		rdword	r0, r7
 112 00a7 2014     		and	r0, #1
  85:nes_gamepad_ctrl.c **** }
 113              		.loc 1 85 0
 114 00a9 02       		lret
 115              	.LFE1
 116              		.global	_player1_left
 117              	_player1_left
 118              	.LFB2
  86:nes_gamepad_ctrl.c **** 
  87:nes_gamepad_ctrl.c **** int player1_left() {
 119              		.loc 1 87 0
  88:nes_gamepad_ctrl.c ****     return (nes_bits & NES0_LEFT) != 0;
 120              		.loc 1 88 0
 121 00aa 670000   		mviw	r7,#_nes_bits
 122 00ad B0       		mov	r0, #0
 123 00ae F2070E04 		rdword	r7, r7
 124 00b2 F9020E60 		test	r7,#0x2 wz
 125 00b6 F301007C 		muxnz	r0,#1
  89:nes_gamepad_ctrl.c **** }
 126              		.loc 1 89 0
 127 00ba 02       		lret
 128              	.LFE2
 129              		.global	_player1_up
 130              	_player1_up
 131              	.LFB3
  90:nes_gamepad_ctrl.c **** 
  91:nes_gamepad_ctrl.c **** int player1_up() {
 132              		.loc 1 91 0
  92:nes_gamepad_ctrl.c ****     return (nes_bits & NES0_UP) != 0;
 133              		.loc 1 92 0
 134 00bb 670000   		mviw	r7,#_nes_bits
 135 00be B0       		mov	r0, #0
 136 00bf F2070E04 		rdword	r7, r7
 137 00c3 F9080E60 		test	r7,#0x8 wz
 138 00c7 F301007C 		muxnz	r0,#1
  93:nes_gamepad_ctrl.c **** }
 139              		.loc 1 93 0
 140 00cb 02       		lret
 141              	.LFE3
 142              		.global	_player1_down
 143              	_player1_down
 144              	.LFB4
  94:nes_gamepad_ctrl.c **** 
  95:nes_gamepad_ctrl.c **** int player1_down() {
 145              		.loc 1 95 0
  96:nes_gamepad_ctrl.c ****     return (nes_bits & NES0_DOWN) != 0;
 146              		.loc 1 96 0
 147 00cc 670000   		mviw	r7,#_nes_bits
 148 00cf B0       		mov	r0, #0
 149 00d0 F2070E04 		rdword	r7, r7
 150 00d4 F9040E60 		test	r7,#0x4 wz
 151 00d8 F301007C 		muxnz	r0,#1
  97:nes_gamepad_ctrl.c **** }
 152              		.loc 1 97 0
 153 00dc 02       		lret
 154              	.LFE4
 155              		.global	_player1_select
 156              	_player1_select
 157              	.LFB5
  98:nes_gamepad_ctrl.c **** 
  99:nes_gamepad_ctrl.c **** int player1_select() {
 158              		.loc 1 99 0
 100:nes_gamepad_ctrl.c ****     return (nes_bits & NES0_SELECT) != 0;
 159              		.loc 1 100 0
 160 00dd 670000   		mviw	r7,#_nes_bits
 161 00e0 B0       		mov	r0, #0
 162 00e1 F2070E04 		rdword	r7, r7
 163 00e5 F9200E60 		test	r7,#0x20 wz
 164 00e9 F301007C 		muxnz	r0,#1
 101:nes_gamepad_ctrl.c **** }
 165              		.loc 1 101 0
 166 00ed 02       		lret
 167              	.LFE5
 168              		.global	_player1_start
 169              	_player1_start
 170              	.LFB6
 102:nes_gamepad_ctrl.c **** 
 103:nes_gamepad_ctrl.c **** int player1_start() {
 171              		.loc 1 103 0
 104:nes_gamepad_ctrl.c ****     return (nes_bits & NES0_START) != 0;
 172              		.loc 1 104 0
 173 00ee 670000   		mviw	r7,#_nes_bits
 174 00f1 B0       		mov	r0, #0
 175 00f2 F2070E04 		rdword	r7, r7
 176 00f6 F9100E60 		test	r7,#0x10 wz
 177 00fa F301007C 		muxnz	r0,#1
 105:nes_gamepad_ctrl.c **** }
 178              		.loc 1 105 0
 179 00fe 02       		lret
 180              	.LFE6
 181              		.global	_player1_b
 182              	_player1_b
 183              	.LFB7
 106:nes_gamepad_ctrl.c **** 
 107:nes_gamepad_ctrl.c **** int player1_b() {
 184              		.loc 1 107 0
 108:nes_gamepad_ctrl.c ****     return (nes_bits & NES0_B) != 0;
 185              		.loc 1 108 0
 186 00ff 670000   		mviw	r7,#_nes_bits
 187 0102 B0       		mov	r0, #0
 188 0103 F2070E04 		rdword	r7, r7
 189 0107 F9400E60 		test	r7,#0x40 wz
 190 010b F301007C 		muxnz	r0,#1
 109:nes_gamepad_ctrl.c **** }
 191              		.loc 1 109 0
 192 010f 02       		lret
 193              	.LFE7
 194              		.global	_player1_fire
 195              	_player1_fire
 196              	.LFB8
 110:nes_gamepad_ctrl.c **** 
 111:nes_gamepad_ctrl.c **** int player1_fire() {
 197              		.loc 1 111 0
 112:nes_gamepad_ctrl.c ****     return (nes_bits & NES0_A) != 0;
 198              		.loc 1 112 0
 199 0110 670000   		mviw	r7,#_nes_bits
 200 0113 B0       		mov	r0, #0
 201 0114 F2070E04 		rdword	r7, r7
 202 0118 F9800E60 		test	r7,#0x80 wz
 203 011c F301007C 		muxnz	r0,#1
 113:nes_gamepad_ctrl.c **** }
 204              		.loc 1 113 0
 205 0120 02       		lret
 206              	.LFE8
 207              		.global	_player1_none
 208              	_player1_none
 209              	.LFB9
 114:nes_gamepad_ctrl.c **** 
 115:nes_gamepad_ctrl.c **** int player1_none() {
 210              		.loc 1 115 0
 116:nes_gamepad_ctrl.c ****     return (nes_bits & (NES0_RIGHT|NES0_LEFT|NES0_UP|NES0_DOWN|NES0_SELECT|NES0_START|NES0_B|NES0_A
 211              		.loc 1 116 0
 212 0121 670000   		mviw	r7,#_nes_bits
 213 0124 B0       		mov	r0, #0
 214 0125 177C     		rdbyte	r7, r7
 215 0127 2702     		cmps	r7, #0 wz,wc
 216 0129 F3010078 		muxz	r0,#1
 117:nes_gamepad_ctrl.c **** }
 217              		.loc 1 117 0
 218 012d 02       		lret
 219              	.LFE9
 220              		.global	_player2_right
 221              	_player2_right
 222              	.LFB10
 118:nes_gamepad_ctrl.c **** 
 119:nes_gamepad_ctrl.c **** int player2_right() {
 223              		.loc 1 119 0
 120:nes_gamepad_ctrl.c ****     return (nes_bits & NES1_RIGHT) != 0;
 224              		.loc 1 120 0
 225 012e 670000   		mviw	r7,#_nes_bits
 226 0131 B0       		mov	r0, #0
 227 0132 F2070E04 		rdword	r7, r7
 228 0136 F9000F60 		test	r7,#0x100 wz
 229 013a F301007C 		muxnz	r0,#1
 121:nes_gamepad_ctrl.c **** }
 230              		.loc 1 121 0
 231 013e 02       		lret
 232              	.LFE10
 233              		.global	_player2_left
 234              	_player2_left
 235              	.LFB11
 122:nes_gamepad_ctrl.c **** 
 123:nes_gamepad_ctrl.c **** int player2_left() {
 236              		.loc 1 123 0
 124:nes_gamepad_ctrl.c ****     return (nes_bits & NES1_LEFT) != 0;
 237              		.loc 1 124 0
 238 013f 670000   		mviw	r7,#_nes_bits
 239 0142 B0       		mov	r0, #0
 240 0143 F2070C04 		rdword	r6, r7
 241 0147 670002   		mviw	r7,#512
 242 014a F8070C60 		test	r6,r7 wz
 243 014e F301007C 		muxnz	r0,#1
 125:nes_gamepad_ctrl.c **** }
 244              		.loc 1 125 0
 245 0152 02       		lret
 246              	.LFE11
 247              		.global	_player2_up
 248              	_player2_up
 249              	.LFB12
 126:nes_gamepad_ctrl.c **** 
 127:nes_gamepad_ctrl.c **** int player2_up() {
 250              		.loc 1 127 0
 128:nes_gamepad_ctrl.c ****     return (nes_bits & NES1_UP) != 0;
 251              		.loc 1 128 0
 252 0153 670000   		mviw	r7,#_nes_bits
 253 0156 B0       		mov	r0, #0
 254 0157 F2070C04 		rdword	r6, r7
 255 015b 670008   		mviw	r7,#2048
 256 015e F8070C60 		test	r6,r7 wz
 257 0162 F301007C 		muxnz	r0,#1
 129:nes_gamepad_ctrl.c **** }
 258              		.loc 1 129 0
 259 0166 02       		lret
 260              	.LFE12
 261              		.global	_player2_down
 262              	_player2_down
 263              	.LFB13
 130:nes_gamepad_ctrl.c **** 
 131:nes_gamepad_ctrl.c **** int player2_down() {
 264              		.loc 1 131 0
 132:nes_gamepad_ctrl.c ****     return (nes_bits & NES1_DOWN) != 0;
 265              		.loc 1 132 0
 266 0167 670000   		mviw	r7,#_nes_bits
 267 016a B0       		mov	r0, #0
 268 016b F2070C04 		rdword	r6, r7
 269 016f 670004   		mviw	r7,#1024
 270 0172 F8070C60 		test	r6,r7 wz
 271 0176 F301007C 		muxnz	r0,#1
 133:nes_gamepad_ctrl.c **** }
 272              		.loc 1 133 0
 273 017a 02       		lret
 274              	.LFE13
 275              		.global	_player2_select
 276              	_player2_select
 277              	.LFB14
 134:nes_gamepad_ctrl.c **** 
 135:nes_gamepad_ctrl.c **** int player2_select() {
 278              		.loc 1 135 0
 136:nes_gamepad_ctrl.c ****     return (nes_bits & NES1_SELECT) != 0;
 279              		.loc 1 136 0
 280 017b 670000   		mviw	r7,#_nes_bits
 281 017e B0       		mov	r0, #0
 282 017f F2070C04 		rdword	r6, r7
 283 0183 670020   		mviw	r7,#8192
 284 0186 F8070C60 		test	r6,r7 wz
 285 018a F301007C 		muxnz	r0,#1
 137:nes_gamepad_ctrl.c **** }
 286              		.loc 1 137 0
 287 018e 02       		lret
 288              	.LFE14
 289              		.global	_player2_start
 290              	_player2_start
 291              	.LFB15
 138:nes_gamepad_ctrl.c **** 
 139:nes_gamepad_ctrl.c **** int player2_start() {
 292              		.loc 1 139 0
 140:nes_gamepad_ctrl.c ****     return (nes_bits & NES1_START) != 0;
 293              		.loc 1 140 0
 294 018f 670000   		mviw	r7,#_nes_bits
 295 0192 B0       		mov	r0, #0
 296 0193 F2070C04 		rdword	r6, r7
 297 0197 670010   		mviw	r7,#4096
 298 019a F8070C60 		test	r6,r7 wz
 299 019e F301007C 		muxnz	r0,#1
 141:nes_gamepad_ctrl.c **** }
 300              		.loc 1 141 0
 301 01a2 02       		lret
 302              	.LFE15
 303              		.global	_player2_b
 304              	_player2_b
 305              	.LFB16
 142:nes_gamepad_ctrl.c **** 
 143:nes_gamepad_ctrl.c **** int player2_b() {
 306              		.loc 1 143 0
 144:nes_gamepad_ctrl.c ****     return (nes_bits & NES1_B) != 0;
 307              		.loc 1 144 0
 308 01a3 670000   		mviw	r7,#_nes_bits
 309 01a6 B0       		mov	r0, #0
 310 01a7 F2070C04 		rdword	r6, r7
 311 01ab 670040   		mviw	r7,#16384
 312 01ae F8070C60 		test	r6,r7 wz
 313 01b2 F301007C 		muxnz	r0,#1
 145:nes_gamepad_ctrl.c **** }
 314              		.loc 1 145 0
 315 01b6 02       		lret
 316              	.LFE16
 317              		.global	_player2_fire
 318              	_player2_fire
 319              	.LFB17
 146:nes_gamepad_ctrl.c **** 
 147:nes_gamepad_ctrl.c **** int player2_fire() {
 320              		.loc 1 147 0
 148:nes_gamepad_ctrl.c ****     return (nes_bits & NES1_A) != 0;
 321              		.loc 1 148 0
 322 01b7 670000   		mviw	r7,#_nes_bits
 323 01ba F2070004 		rdword	r0, r7
 324 01be 301090   		shl	r0, #16
 325 01c1 301FA0   		shr	r0, #31
 149:nes_gamepad_ctrl.c **** }
 326              		.loc 1 149 0
 327 01c4 02       		lret
 328              	.LFE17
 329              		.global	_player2_none
 330              	_player2_none
 331              	.LFB18
 150:nes_gamepad_ctrl.c **** 
 151:nes_gamepad_ctrl.c **** int player2_none() {
 332              		.loc 1 151 0
 152:nes_gamepad_ctrl.c ****     return (nes_bits & (NES1_RIGHT|NES1_LEFT|NES1_UP|NES1_DOWN|NES1_SELECT|NES1_START|NES1_B|NES1_A
 333              		.loc 1 152 0
 334 01c5 670000   		mviw	r7,#_nes_bits
 335 01c8 B0       		mov	r0, #0
 336 01c9 F2070C04 		rdword	r6, r7
 337 01cd 6700FF   		mviw	r7,#65280
 338 01d0 F8070C60 		test	r6,r7 wz
 339 01d4 F3010078 		muxz	r0,#1
 153:nes_gamepad_ctrl.c **** }
 340              		.loc 1 153 0
 341 01d8 02       		lret
 342              	.LFE18
 343              		.local	_nes_bits
 344              		.comm	_nes_bits,2,2
 516              	.Letext0
 517              		.file 2 "c:\\program files (x86)\\simpleide\\propeller-gcc\\bin\\../lib/gcc/propeller-elf/4.6.1/..
 518              		.file 3 "c:\\program files (x86)\\simpleide\\propeller-gcc\\bin\\../lib/gcc/propeller-elf/4.6.1/..
DEFINED SYMBOLS
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:2      .text:00000000 .Ltext0
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:4      .text:00000000 _read_gamepad
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:5      .text:00000000 .LFB0
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:8      .text:00000000 L0
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:344    .bss:00000000 _nes_bits
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:104    .text:000000a0 .LFE0
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:106    .text:000000a0 _player1_right
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:107    .text:000000a0 .LFB1
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:115    .text:000000aa .LFE1
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:117    .text:000000aa _player1_left
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:118    .text:000000aa .LFB2
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:128    .text:000000bb .LFE2
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:130    .text:000000bb _player1_up
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:131    .text:000000bb .LFB3
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:141    .text:000000cc .LFE3
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:143    .text:000000cc _player1_down
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:144    .text:000000cc .LFB4
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:154    .text:000000dd .LFE4
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:156    .text:000000dd _player1_select
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:157    .text:000000dd .LFB5
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:167    .text:000000ee .LFE5
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:169    .text:000000ee _player1_start
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:170    .text:000000ee .LFB6
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:180    .text:000000ff .LFE6
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:182    .text:000000ff _player1_b
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:183    .text:000000ff .LFB7
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:193    .text:00000110 .LFE7
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:195    .text:00000110 _player1_fire
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:196    .text:00000110 .LFB8
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:206    .text:00000121 .LFE8
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:208    .text:00000121 _player1_none
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:209    .text:00000121 .LFB9
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:219    .text:0000012e .LFE9
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:221    .text:0000012e _player2_right
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:222    .text:0000012e .LFB10
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:232    .text:0000013f .LFE10
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:234    .text:0000013f _player2_left
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:235    .text:0000013f .LFB11
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:246    .text:00000153 .LFE11
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:248    .text:00000153 _player2_up
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:249    .text:00000153 .LFB12
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:260    .text:00000167 .LFE12
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:262    .text:00000167 _player2_down
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:263    .text:00000167 .LFB13
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:274    .text:0000017b .LFE13
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:276    .text:0000017b _player2_select
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:277    .text:0000017b .LFB14
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:288    .text:0000018f .LFE14
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:290    .text:0000018f _player2_start
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:291    .text:0000018f .LFB15
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:302    .text:000001a3 .LFE15
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:304    .text:000001a3 _player2_b
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:305    .text:000001a3 .LFB16
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:316    .text:000001b7 .LFE16
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:318    .text:000001b7 _player2_fire
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:319    .text:000001b7 .LFB17
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:328    .text:000001c5 .LFE17
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:330    .text:000001c5 _player2_none
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:331    .text:000001c5 .LFB18
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:342    .text:000001d9 .LFE18
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:346    .debug_frame:00000000 .Lframe0
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:516    .text:000001d9 .Letext0
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:520    .debug_info:00000000 .Ldebug_info0
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:854    .debug_abbrev:00000000 .Ldebug_abbrev0
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:999    .debug_loc:00000000 .LLST0
C:\Users\lili\AppData\Local\Temp\ccxkguuZ.s:1020   .debug_line:00000000 .Ldebug_line0

UNDEFINED SYMBOLS
DIRA
OUTA
INA
__MASK_0000FFFF
