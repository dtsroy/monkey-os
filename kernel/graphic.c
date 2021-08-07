#include "kernel/Graphic.h"

void init_screen(unsigned char *vram, int xs, int ys) {
	draw_box(vram, xs, 0, 0, 0, xs, ys);//主桌面
	put_str(vram, xs, 0, 0, 7, "Hello from MonkeyOS.");
}

void init_palette(void){
	static unsigned char table_rgb[17 * 3] = {
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
		0x00, 0x84, 0x84,	/* 14:浅暗蓝 */
		0x84, 0x84, 0x84, 	/* 15:暗灰 */ //窗口顶端
		0x32, 0x32, 0x32    /* 16:窗口底板 */
	};
	set_palette(0, 16, table_rgb);
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
	for (y=y0; y<y1; y++) {
		for (x=x0; x<x1; x++) {
			vram[x+y*xs] = color;
		}
	}
}

void init_pointer(unsigned char *msbuf, int bg) {
	static char pointer[12][14] = {
		"*.............",
		"*0*...........",
		"*000*.........",
		"*00000*.......",
		"*0000000*.....",
		"*000000000*...",
		"*00000000000*.",
		"*000*0000*****",
		"*00*.*000*....",
		"*0*...*000*...",
		"**.....*000*..",
		"........*****."
	};
	int x, y;
	for (y=0; y<12; y++) {
		for (x=0; x<12; x++) {
			char now = pointer[y][x];
			if (now == '*') {
				msbuf[x+y*12] = 7;
			}
			if (now == '0') {
				msbuf[x+y*12] = 0;
			}
			if (now == '.') {
				msbuf[x+y*12] = bg;
			}
		}
	}
}

void draw_block(unsigned char *vram, unsigned char *buf,
				int vxs, int xs, int ys, int x0, int y0, int bxs) {
	for (int y=0; y<ys; y++) {
		for (int x=0; x<xs; x++) {
			vram[(x0+x)+(y0+y)*vxs] = buf[x+y*bxs];
		}
	}
}

void put_font(unsigned char *vram, int xsize, int x, int y, char color, unsigned char *font) {
	int i;
	char *vaddr, dat;
	for (i=0; i<16; i++) {
		vaddr = vram + (y+i) * xsize + x;
		dat = font[i];
		if ((dat & 0x80) != 0) {vaddr[0] = color;}
		if ((dat & 0x40) != 0) {vaddr[1] = color;}
		if ((dat & 0x20) != 0) {vaddr[2] = color;}
		if ((dat & 0x10) != 0) {vaddr[3] = color;}
		if ((dat & 0x08) != 0) {vaddr[4] = color;}
		if ((dat & 0x04) != 0) {vaddr[5] = color;}
		if ((dat & 0x02) != 0) {vaddr[6] = color;}
		if ((dat & 0x01) != 0) {vaddr[7] = color;}
	}
}

void put_str(unsigned char *vram, int xsize, int x, int y, char color, char *str) {
	extern char MonkeyOSMainFont[4096];
	for (; *str!=0x00; str++) {
		put_font(vram, xsize, x, y, color, MonkeyOSMainFont + *str * 16);
		x += 8;
	}
}
