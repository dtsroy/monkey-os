#ifndef MKPACK_H
#define MKPACK_H

#include <stdio.h>

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
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
void ihr21x(void);
void ihr27x(void);
void ihr2cx(void);
void ihr20x(void);
void io_sti(void);
void io_shlt(void);
int io_inp8(int port);
int load_cr0(void);
void save_cr0(int cr0);
int getmemx(int start, int end);
void _shutdown(void);

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
#define EFLAGS_AC_BIT 0x00040000

void fin(void);
unsigned char test486(void);

struct textinfo {
	char *text;
	int bg, fg;
	int len;
};
struct textinfo packText(char *text, int bg, int fg, int len);

//init_dt.c
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
#define PIC0_ICW1 0x0020
#define PIC0_OCW 0x0020
#define PIC0_IMR 0x0021
#define PIC0_ICW2 0x0021
#define PIC0_ICW3 0x0021
#define PIC0_ICW4 0x0021
#define PIC1_ICW1 0x00a0
#define PIC1_OCW 0x00a0
#define PIC1_IMR 0x00a1
#define PIC1_ICW2 0x00a1
#define PIC1_ICW3 0x00a1
#define PIC1_ICW4 0x00a1
void init_pic(void);
void ihr21(int *esp);
void ihr27(int *esp);
void ihr2c(int *esp);
void ihr20(int *esp);

//fifo.c
struct fifo {
	int *addr; //缓冲区地址
	int wp, rp, sz, free, flag; //写入指针,读取指针,大小,空余,是否溢出(0, -1)
};

void init_fifo(struct fifo *xmain, int size, unsigned int *buf);
int fifo_put(struct fifo *xmain, int dat);
int fifo_get(struct fifo *xmain);
int fifo_sts(struct fifo *xmain);

//keyboard_and_mouse.c
#define P_KEYDAT 0x0060
#define P_KEYSTA 0x0064
#define P_KEYCMD 0x0064
#define KEYCMD_WRITE_MODE 0x60
#define KBC_MODE 0x47
#define KEYCMD_SENDTO_MOUSE 0xd4
#define MOUSECMD_ENABLE 0xf4

#define K_DT0 256
#define M_DT0 512
struct mdec {
	unsigned char buf[3], st;
	int x, y, btn;
};

void wait_kr(void);
void init_keyboard(void);
void init_mouse(void);
int mdecode(struct mdec *xmain, unsigned char *dat);

//memory.c
#define MCTRLER_MAX_FREES 4096
#define MCTRLER_ADDR 0x3c0000 //内存控制地址
struct freeif {
	unsigned addr, size;
};
struct mctrler {
	int frees, maxfrees, losts, lostsize;
	struct freeif free[MCTRLER_MAX_FREES];
};


#define CR0_CACHE_DISABLE 0x60000000
void init_mctrler(struct mctrler *xmain);
unsigned int mctrler_total(struct mctrler *xmain);
unsigned int mctrler_alloc(struct mctrler *xmain, unsigned int size);
int mctrler_free(struct mctrler *xmain, unsigned int addr, unsigned int size);
unsigned int mctrler_allocx(struct mctrler *xmain, unsigned int size);
int mctrler_freex(struct mctrler *xmain, unsigned int addr, unsigned int size);

//sheet.c
#define MAX_SHEETS 256
struct sheet {
	unsigned char *buf;
	struct sctrler *scr;
	int bxs, bys, vx0, vy0, cliv, height, flag;
};
struct sctrler {
	unsigned char *vram, *map;
	int xs, ys, top;
	struct sheet *shts[MAX_SHEETS];
	struct sheet shts0[MAX_SHEETS];
};

struct sctrler *init_sctrler(struct mctrler *xmain, unsigned int vram, int xs, int ys);
struct sheet *sctrler_alloc(struct sctrler *xmain);

void sctrler_refreshx(struct sctrler *xmain, int vx0, int vy0, int vx1, int vy1, int h0, int h1);
void sctrler_refreshmap(struct sctrler *xmain, int vx0, int vy0, int vx1, int vy1, int h0);

void sheet_refresh(struct sheet *sht, int bx0, int by0, int bx1, int by1);
void sheet_setbuf(struct sheet *xmain, unsigned char *buf, int xs, int ys, int cliv);
void sheet_setheight(struct sheet *sht, int height);
void sheet_free(struct sheet *sht);
void sheet_slide(struct sheet *sht, int vx0, int vy0);
void sheet_put_str(struct sheet *xmain, int x, int y, int bg, int fg, char *text, int len);

//window.c
struct mwindow {
	char *title;
	struct sheet *sht;
	int xs, ys;
};

struct mwindow *init_mwindow(char *title, struct sheet *sht, int xs, int ys);
void mwindow_draw(struct mwindow *xmain);

//timer.c
#define TIMER_F_ALLOC 1
#define TIMER_F_USING 2

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040
struct timer {
	struct timer *next;
	unsigned int timeout, flags;
	int data;
};
#define MAX_TIMERS 500
struct tctrler {
	unsigned int count, next;
	struct timer *t0;
	struct timer timers[MAX_TIMERS];
};

void init_pit(void);
struct timer *timer_alloc(void);
void timer_free(struct timer *xmain);
void timer_init(struct timer *xmain, int data);
void timer_set(struct timer *xmain, unsigned int timeout);

#endif
