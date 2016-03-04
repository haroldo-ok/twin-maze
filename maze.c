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

#define DIRECTION_EAST 0
#define DIRECTION_NORTH 1
#define DIRECTION_WEST 2
#define DIRECTION_SOUTH 3

typedef struct _actor {
	char x, y;
	char dir;
} actor;

char window_canvas[WINDOW_WIDTH][WINDOW_HEIGHT];

actor player;

const char map[][8] = {
	"#######",
	"#.....#",
	"#.###.#",
	"#.....#",
	"#######",
};

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

void draw_canvas_char(char x, char y, char c) {
	if (x >= 0 && x < WINDOW_WIDTH && y >= 0 && y < WINDOW_HEIGHT) {
		window_canvas[x][y] = c;
	}
}

void draw_front(unsigned char z, char pos) {
	char x1 = WINDOW_CENTER_X + pos - z;
	char x2 = x1 + z + z;
	char y1 = WINDOW_CENTER_Y - z;
	char y2 = y1 + z + z - 1;
	char xi, yi;
	char ch = '*';

	for (xi = x1; xi != x2; xi++) {
		draw_canvas_char(xi, y1, '=');
		draw_canvas_char(xi, y2, '=');
		for (yi = y1 + 1; yi < y2; yi++) {
			draw_canvas_char(xi, yi, ch);
		}
	}
}

void draw_side(unsigned char z1, unsigned char z2, char pos) {
	char incr = pos < 0 ? -1 : 1;
	char x1 = WINDOW_CENTER_X + (pos < 0 ?  -z1 - 1 : z1);
	char x2 = WINDOW_CENTER_X + (pos < 0 ?  -z2 - 1 : z2);
	char y1 = WINDOW_CENTER_Y - z1 - 1;
	char y2 = WINDOW_CENTER_Y + z1;
	char ch0 = '|';
	char cht = pos < 0 ? '\\' : '/';
	char chb = pos < 0 ? '/' : '\\';
	char xi, yi;

	if (z2 <= z1) {
		return;
	}

	for (xi = x1; xi != x2; xi += incr) {
		draw_canvas_char(xi, y1, cht);
		draw_canvas_char(xi, y2, chb);
		for (yi = y1 + 1; yi < y2; yi++) {
			draw_canvas_char(xi, yi, ch0);
		}
		y1--, y2++;
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

	draw_side(0, 7, -1);
	draw_side(0, 7, 1);

	draw_window_canvas(x, y);
}

char project_x(char x, char y, char direction) {
	switch (direction) {
		case DIRECTION_NORTH:
			return x;
 		case DIRECTION_SOUTH:
			return -x;
		case DIRECTION_EAST:
			return y;
		case DIRECTION_WEST:
			return -y;
	}

	return 0;
}

char project_y(char x, char y, char direction) {
	switch (direction) {
		case DIRECTION_NORTH:
			return -y;
 		case DIRECTION_SOUTH:
			return y;
		case DIRECTION_EAST:
			return x;
		case DIRECTION_WEST:
			return -x;
	}

	return 0;
}

char get_map_at(char x, char y, char dx, char dy, char dir) {
	x += project_x(dx, dy, dir);
	y += project_y(dx, dy, dir);
	return map[y][x];
}

void main(void) {
	char timer, z, prev_z;
	char y, t_x, t_y;
	unsigned int kp;

	load_palette();
	load_font();
	SMS_displayOn();

	player.x = 1;
	player.y = 2;
	player.dir = DIRECTION_SOUTH;

	timer = 0;
	while (true) {
		kp = SMS_getKeysStatus();

		if (kp & PORT_A_KEY_UP) {
			player.y++;
		}
		if (kp & PORT_A_KEY_DOWN) {
			player.y--;
		}
		if (kp & PORT_A_KEY_LEFT) {
			player.dir++;
		}
		if (kp & PORT_A_KEY_RIGHT) {
			player.dir--;
		}
		player.dir &= 0x03;

		clear_canvas();

		/*
		// Left
		z = timer;
		draw_side(0, z, -1);
		draw_front(z, -(z + z));
		draw_side(z + z, 7, -1);

		// Right
		z = (timer + 3) % 7;
		draw_side(0, z, 1);
		draw_front(z, (z + z));
		draw_side(z + z, 7, 1);
		*/
		for (y = 3, z = 1, prev_z = 0; y > -1; y--, prev_z = z, z += z) {
			// Left
			if (get_map_at(player.x, player.y, -1, y, player.dir) == '#') {
				draw_front(z, -(z + z));
				draw_side(prev_z, z, -1);
			}

			// Right
			if (get_map_at(player.x, player.y, 1, y, player.dir) == '#') {
				draw_front(z, (z + z));
				draw_side(prev_z, z, 1);
			}

			// Center
			if (get_map_at(player.x, player.y, 0, y, player.dir) == '#') {
				draw_front(z, 0);
			}
		}

		SMS_waitForVBlank();
		draw_window_canvas(1, 1);

		timer = (timer + 1) % 7;
	}

}

SMS_EMBED_SEGA_ROM_HEADER(9999,0); // code 9999 hopefully free, here this means 'homebrew'
SMS_EMBED_SDSC_HEADER(0,1, 2016,2,29, "Haroldo-OK\\2016", "Twin Maze",
  "A first person maze game.\n"
  "Built using devkitSMS & SMSlib - https://github.com/sverx/devkitSMS");
