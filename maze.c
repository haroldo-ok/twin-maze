#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "SMSlib/src/SMSlib.h"
#include "PSGlib/src/PSGlib.h"
#include "gfx.h"

#define WINDOW_WIDTH 14
#define WINDOW_HEIGHT 14
#define WINDOW_CENTER_X (WINDOW_WIDTH >> 1)
#define WINDOW_CENTER_Y (WINDOW_HEIGHT >> 1)

char window_canvas[WINDOW_WIDTH][WINDOW_HEIGHT];

void load_palette() {
	unsigned char i;

	for (i=0;i<16;i++) {
    SMS_setBGPaletteColor(i,0x00);    // black
	}
  SMS_setBGPaletteColor(01,0x3f);     // white
}

void load_font(void) {
	unsigned char i, j;
	unsigned char buffer[32], *o, *d;

	o = font_fnt;
	for (i = 0; i != 96; i++) {
		d = buffer;
		for (j = 0; j != 8; j++) {
			*d = *o; d++;	o++;
			*d = 0;	d++;
			*d = 0;	d++;
			*d = 0;	d++;
		}
		SMS_loadTiles(buffer, i, 32);
	}
}

void draw_char(unsigned char x, unsigned char y, char c) {
  SMS_setTileatXY(x, y, c - 32);
}

void draw_canvas_char(unsigned char x, unsigned char y, char c) {
	window_canvas[x][y] = c;
}

void draw_front(unsigned char z, char pos) {
	char x1 = WINDOW_CENTER_X + pos - z;
	char x2 = x1 + z + z;
	char y1 = WINDOW_CENTER_Y - z;
	char y2 = y1 + z + z - 1;
	char xi, yi;
	char ch = '0' + z;

	for (xi = x1; xi != x2; xi++) {
		draw_canvas_char(xi, y1, '=');
		draw_canvas_char(xi, y2, '=');
		for (yi = y1 + 1; yi < y2; yi++) {
			draw_canvas_char(xi, yi, ch);
		}
	}
}

void clear_canvas() {
	memset(window_canvas[0], ' ', WINDOW_WIDTH * WINDOW_HEIGHT);
}

void draw_window_canvas(unsigned char x, unsigned char y) {
		unsigned int buffer[WINDOW_WIDTH], *p_b;
		char cy, cx, *p_l, *p_c;

		for (cy = WINDOW_HEIGHT, p_l = window_canvas[0]; cy; cy--, p_l++) {
			for (cx = WINDOW_WIDTH, p_b = buffer, p_c = p_l; cx; cx--, p_b++, p_c += WINDOW_HEIGHT) {
				*p_b = *p_c - 32;
			}

			SMS_loadTileMap(x, y, buffer, WINDOW_WIDTH << 1);
			y++;
		}
}

void draw_window(unsigned char x, unsigned char y) {
	unsigned char i;

	for (i = 0; i != WINDOW_CENTER_X; i++) {
		draw_canvas_char(i, i, '\\');
		draw_canvas_char(i, WINDOW_HEIGHT - i - 1, '/');
		draw_canvas_char(WINDOW_WIDTH - i - 1, i, '/');
		draw_canvas_char(WINDOW_HEIGHT - i - 1, WINDOW_HEIGHT - i - 1, '\\');
	}

	draw_window_canvas(x, y);
}

void main(void) {
	char i;

	load_palette();
	load_font();
	SMS_displayOn();

	draw_window(1, 1);
	draw_window(17, 1);

//	draw_front(1, 1, 3, 0);

	i = 0;
	while (true) {
		clear_canvas();

		SMS_waitForVBlank();
		draw_window(1, 1);
		draw_front(i + 1, 0);
		draw_window_canvas(1, 1);

		i = (i + 1) % 7;
	}

}

SMS_EMBED_SEGA_ROM_HEADER(9999,0); // code 9999 hopefully free, here this means 'homebrew'
SMS_EMBED_SDSC_HEADER(0,1, 2016,2,29, "Haroldo-OK\\2016", "Twin Maze",
  "A first person maze game.\n"
  "Built using devkitSMS & SMSlib - https://github.com/sverx/devkitSMS");
