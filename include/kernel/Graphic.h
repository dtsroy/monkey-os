#ifndef GRAPHIC_KERNEL_H
#define GRAPHIC_KERNEL_H

#include "kernel/Functions.h"

// 以下注意: vram和xs的保护是为了能够在图层缓冲上绘制
void init_palette(void);
void set_palette(int start, int end, unsigned char *tb);
void draw_box(unsigned char *vram, int xs, unsigned char color, int x0, int y0, int x1, int y1);
void init_pointer(unsigned char *msbuf, int bg);
void draw_block(unsigned char *vram, unsigned char *buf, int vxs, int xs, int ys, int x0, int y0, int bxs);
void put_str(unsigned char *vram, int xsize, int x, int y, char color, char *str);
void put_font(unsigned char *vram, int xsize, int x, int y, char color, unsigned char *font);

#endif
