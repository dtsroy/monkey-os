#ifndef MKPACK_H
#define MKPACK_H

struct BootInfo
{
	char cyls, leds, vmode, reserve;
	short xs, ys;
	char *vram;
};

//func.nas
void io_hlt(void);
void io_outp8(int port, int data);
void io_cli(void);
int io_load_eflags(void);
void io_save_eflags(int eflags);

//graghic.c
void init_palette(void);
void set_palette(int start, int end, unsigned char *tb);
void init_screen(unsigned char *vram, int xs, int ys);
void draw_box(unsigned char *vram, int xs, unsigned char color, int x0, int y0, int x1, int y1);

//tools.c
void fin(void);

#endif
