#ifndef MKPACK_H
#define MKPACK_H

#include <stdio.h>

struct BootInfo
{
	char cyls, leds, vmode, reserve;
	short xs, ys;
	char *vram;
};

//GDT, IDT相关
struct SEGMENT_DESCRIPTOR {
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};
struct GATE_DESCRIPTOR {
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};

//func.nas
void io_hlt(void);
void io_outp8(int port, int data);
void io_cli(void);
int io_load_eflags(void);
void io_save_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
void ihr21x(void);
void ihr27x(void);
void io_sti(void);

//graghic.c
void init_palette(void);
void set_palette(int start, int end, unsigned char *tb);
void init_screen(unsigned char *vram, int xs, int ys);
void draw_box(unsigned char *vram, int xs, unsigned char color, int x0, int y0, int x1, int y1);
void init_pointer(unsigned char *msbuf, int bg);
void draw_block(unsigned char *vram, unsigned char *buf, int vxs, int xs, int ys, int x0, int y0, int bxs);
void put_str(unsigned char *vram, int xsize, int x, int y, char color, char *str);
void put_font(unsigned char *vram, int xsize, int x, int y, char color, unsigned char *font);

//tools.c
void fin(void);

//init_dt.c
#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_INTGATE32	0x008e
void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

//int.c
#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1
void init_pic(void);
void ihr21(int *esp);
void ihr27(int *esp);

#endif
