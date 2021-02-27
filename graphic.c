#include "mkpack.h"

void init_screen(unsigned char *vram, int xs, int ys) {
	draw_box(vram, xs, 10, 0, 0, xs - 1, ys - 16);//主桌面
	draw_box(vram, xs, 7, 0, ys - 15, xs - 1, ys); //任务栏主体
	draw_box(vram, xs, 0, 0, ys - 15, xs - 305, ys); //左下角logo(虽然没图片)
	draw_box(vram, xs, 0, xs - 3, ys - 15, xs, ys); //右下角return_to_desktop
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

void draw_point(unsigned char *vram, )

