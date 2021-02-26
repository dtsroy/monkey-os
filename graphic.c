#include "mkpack.h"

void init_screen(unsigned char *vram, int xs, int ys) {
	draw_box(vram, xs, 10,  0,     0,      xs -  1, ys - 27);//主桌面
	// draw_box(vram, xs, 13,  0,     ys - 28, xs -  1, ys - 28);
	// draw_box(vram, xs, 12,  0,     ys - 27, xs -  1, ys - 27);
	draw_box(vram, xs, 11,  0,     ys - 26, xs -  1, ys -  1);

	draw_box(vram, xs, 10,  3,     ys - 24, 59,     ys - 24);
	draw_box(vram, xs, 9,  2,     ys - 24,  2,     ys -  4);
	draw_box(vram, xs, 8,  3,     ys -  4, 59,     ys -  4);
	draw_box(vram, xs, 7, 59,     ys - 23, 59,     ys -  5);
	draw_box(vram, xs, 6,  2,     ys -  3, 59,     ys -  3);
	draw_box(vram, xs, 5, 60,     ys - 24, 60,     ys -  3);

	draw_box(vram, xs, 4, xs - 47, ys - 24, xs -  4, ys - 24);
	draw_box(vram, xs, 3, xs - 47, ys - 23, xs - 47, ys -  4);
	draw_box(vram, xs, 2, xs - 47, ys -  3, xs -  4, ys -  3);
	draw_box(vram, xs, 1, xs -  3, ys - 24, xs -  3, ys -  3);
	return;
}

void init_palette(void){
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0:黑 */
		0xff, 0x00, 0x00,	/*  1:梁红 */
		0x00, 0xff, 0x00,	/*  2:亮绿 */
		0xff, 0xff, 0x00,	/*  3:亮黄 */
		0x00, 0x00, 0xff,	/*  4:亮蓝 */
		0xff, 0x00, 0xff,	/*  5:亮紫 */
		0x00, 0xff, 0xff,	/*  6:浅亮蓝 */
		0xff, 0xff, 0xff,	/*  7:白 */
		0xc6, 0xc6, 0xc6,	/*  8:亮灰 */
		0x84, 0x00, 0x00,	/*  9:暗红 */
		0x00, 0x84, 0x00,	/* 10:暗绿 */
		0x84, 0x84, 0x00,	/* 11:暗黄 */
		0x00, 0x00, 0x84,	/* 12:暗青 */
		0x84, 0x00, 0x84,	/* 13:暗紫 */
		0x00, 0x84, 0x84,	/* 14:浅暗蓝 */ //008484
		0x84, 0x84, 0x84 	/* 15:暗灰 */ //848484
	};
	set_palette(0, 15, table_rgb);
	return;
}

void set_palette(int start, int end, unsigned char *tb) {
	int i, eflags;
	eflags = io_load_eflags();
	io_cli(); //禁止中断
	io_outp8(0x03c8, start);
	for (i=start; i<=end; i++) {
		io_outp8(0x03c9, tb[0] / 4);
		io_outp8(0x03c9, tb[1] / 4);
		io_outp8(0x03c9, tb[2] / 4);
		tb += 3;
	}
	io_save_eflags(eflags);
	return;
}

void draw_box(unsigned char *vram, int xs, unsigned char color, int x0, int y0, int x1, int y1) {
	int x, y;
	for (y=y0; y<=y1; y++) {
		for (x=x0; x<=x1; x++) {
			vram[x+y*xs] = color;
		}
	}
	return;
}

