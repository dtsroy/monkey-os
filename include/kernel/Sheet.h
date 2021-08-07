#ifndef SHEET_KERNEL_H
#define SHEET_KERNEL_H

#include "kernel/Functions.h"
#include "kernel/Memory.h"
#include "kernel/Graphic.h"

#define MAX_SHEETS 256

struct sheet {
	unsigned char *buf;
	int bxs, bys, vx0, vy0, cliv, height, flag;
};

struct sctrler {
	unsigned char *map;
	int top;
	struct sheet *shts[MAX_SHEETS];
	struct sheet shts0[MAX_SHEETS];
};

void init_sctrler(void);
struct sheet *sctrler_alloc(void);
void sctrler_refreshx(int vx0, int vy0, int vx1, int vy1, int h0, int h1);
void sctrler_refreshmap(int vx0, int vy0, int vx1, int vy1, int h0);
void sheet_refresh(struct sheet *sht, int bx0, int by0, int bx1, int by1);
void sheet_setbuf(struct sheet *xmain, unsigned char *buf, int xs, int ys, int cliv);
void sheet_setheight(struct sheet *sht, int height);
void sheet_free(struct sheet *sht);
void sheet_slide(struct sheet *sht, int vx0, int vy0);
void sheet_put_str(struct sheet *xmain, int x, int y, int bg, int fg, char *text, int len);

#endif
