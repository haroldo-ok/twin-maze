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

void draw_front(unsigned char x, unsigned char y, unsigned char z, char pos) {
	char x1 = x + WINDOW_CENTER_X + pos - z;
	char x2 = x1 + z + z;
	char y1 = y + WINDOW_CENTER_Y - z;
	char y2 = y1 + z + z - 1;
	char xi, yi;
	char ch = '0' + z;

	for (xi = x1; xi != x2; xi++) {
		draw_char(xi, y1, '=');
		draw_char(xi, y2, '=');
		for (yi = y1 + 1; yi < y2; yi++) {
			draw_char(xi, yi, ch);
		}
	}
}

void draw_window(unsigned char x, unsigned char y) {
	unsigned char i;

	for (i = 0; i != WINDOW_CENTER_X; i++) {
		draw_char(x + i, y + i, '\\');
		draw_char(x + i, y + WINDOW_HEIGHT - i - 1, '/');
		draw_char(x + WINDOW_WIDTH - i - 1, y + i, '/');
		draw_char(x + WINDOW_HEIGHT - i - 1, y + WINDOW_HEIGHT - i - 1, '\\');
	}

}

void main(void) {
	char i;

	load_palette();
	load_font();
	SMS_displayOn();

	draw_window(1, 1);
	draw_window(17, 1);

	draw_front(1, 1, 3, 0);

	i = 0;
	while (true) {
		SMS_waitForVBlank();
		draw_window(1, 1);
		draw_front(1, 1, i + 1, 0);

		i = (i + 1) % 7;
	}

}

SMS_EMBED_SEGA_ROM_HEADER(9999,0); // code 9999 hopefully free, here this means 'homebrew'
SMS_EMBED_SDSC_HEADER(0,1, 2016,2,29, "Haroldo-OK\\2016", "Twin Maze",
  "A first person maze game.\n"
  "Built using devkitSMS & SMSlib - https://github.com/sverx/devkitSMS");
