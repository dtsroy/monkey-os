#include "kernel/Sheet.h"

extern struct BootInfo *btif;
extern struct sctrler *scr;

void init_sctrler(void) {
	scr = mctrler_allocx(sizeof(struct sctrler));
	int i;
	if (scr == 0) {return;}
	scr->map = (unsigned char *) mctrler_allocx(btif->xs * btif->ys);
	if (scr->map == 0) {return;}
	// scr->vram = vram;
	// btif->xs = xs;
	// btif->ys = ys;
	scr->top = -1;
	for (i=0; i<MAX_SHEETS; i++) {
		scr->shts0[i].flag = 0;
	}
}

struct sheet *sctrler_alloc(void) {
	struct sheet *ret;
	int i;
	for (i=0; i<MAX_SHEETS; i++) {
		if (scr->shts0[i].flag == 0) {
			ret = &scr->shts0[i];
			ret->flag = 1;
			ret->height = -1;
			// ret->scr = xmain;
			return ret;
		}
	}
	return 0;
}

void sheet_setbuf(struct sheet *xmain, unsigned char *buf, int xs, int ys, int cliv) {
	xmain->buf = buf;
	xmain->bxs = xs;
	xmain->bys = ys;
	xmain->cliv = cliv;
}

void sheet_setheight(struct sheet *sht, int height) {
	int tmp, old = sht->height;
	//调整高度
	if (height > scr->top + 1) {
		height = scr->top + 1;
	}
	if (height < -1) {
		height = -1;
	}
	//设定
	sht->height = height;
	//针对指针排列
	if (height < old) {
		//比以前低
		if (height >= 0) {
			//得到显示机会
			for (tmp=old; tmp>height; tmp--) {
				//old及以后后移
				scr->shts[tmp] = scr->shts[tmp - 1];
				scr->shts[tmp]->height = tmp;
			}
			scr->shts[height] = sht;
			sctrler_refreshmap(sht->vx0, sht->vy0, sht->vx0 + sht->bxs, sht->vy0 + sht->bys, height+1);
			sctrler_refreshx(sht->vx0, sht->vy0, sht->vx0 + sht->bxs, sht->vy0 + sht->bys, height+1, old);
		} else {
			if (scr->top > old) {
				//去掉old,再前移
				for (tmp=old; tmp<scr->top; tmp++) {
					scr->shts[tmp] = scr->shts[tmp+1];
					scr->shts[tmp]->height = tmp;
				}
			}
			//显示中被删了一个,top-1
			sctrler_refreshmap(sht->vx0, sht->vy0, sht->vx0 + sht->bxs, sht->vy0 + sht->bys, 0);
			sctrler_refreshx(sht->vx0, sht->vy0, sht->vx0 + sht->bxs, sht->vy0 + sht->bys, 0, old-1);
			scr->top--;
		}

	} else if (height > old) {
		//比以前高
		if (old >= 0) {
			//本来就在显示
			for (tmp=old; tmp<height; tmp++) {
				scr->shts[tmp] = scr->shts[tmp + 1];
				scr->shts[tmp]->height = tmp;
			}
			scr->shts[height] = sht;
		} else {
			//本来为显示
			for (tmp=scr->top; tmp>=height; tmp--) {
				scr->shts[tmp + 1] = scr->shts[tmp];
				scr->shts[tmp + 1]->height = tmp + 1;
			}
			scr->shts[height] = sht;
			scr->top++;
		}
		sctrler_refreshmap(sht->vx0, sht->vy0, sht->vx0 + sht->bxs, sht->vy0 + sht->bys, height);
		sctrler_refreshx(sht->vx0, sht->vy0, sht->vx0 + sht->bxs, sht->vy0 + sht->bys, height, height);
	}
}

void sheet_refresh(struct sheet *sht, int bx0, int by0, int bx1, int by1) {
	if (sht->height >= 0) {
		//显示中
		sctrler_refreshx(sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1, sht->height, sht->height);
	}
}

void sctrler_refreshx(int vx0, int vy0, int vx1, int vy1, int h0, int h1) {
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned char *buf, c, *vram=btif->vram, *map = scr->map, sid;
	struct sheet *sht;

	if (vx0 < 0) {vx0 = 0;}
	if (vy0 < 0) {vy0 = 0;}
	if (vx1 > btif->xs) {vx1 = btif->xs;}
	if (vy1 > btif->ys) {vy1 = btif->ys;}

	for (h=0; h<=scr->top; h++) {
		sht = scr->shts[h];
		buf = sht->buf;
		sid = sht - scr->shts0;
		//推到刷新范围
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		//刷新范围在图层外
		if (bx0 < 0) {bx0 = 0;}
		if (by0 < 0) {by0 = 0;}
		//重叠
		if (bx1 > sht->bxs) {bx1 = sht->bxs;}
		if (by1 > sht->bys) {by1 = sht->bys;}

		for (by=by0; by<by1; by++) {
			vy = sht->vy0 + by;
			for (bx=bx0; bx<bx1; bx++) {
				vx = sht->vx0 + bx;
				c = buf[by * sht->bxs + bx];
				if (map[vy * btif->xs + vx] == sid) {
					vram[vy * btif->xs + vx] = c;
				}
			}
		}
	}
}

void sctrler_refreshmap(int vx0, int vy0, int vx1, int vy1, int h0) {
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned char *buf, sid, *map = scr->map;
	struct sheet *sht;

	if (vx0 < 0) { vx0 = 0; }
	if (vy0 < 0) { vy0 = 0; }
	if (vx1 > btif->xs) { vx1 = btif->xs; }
	if (vy1 > btif->ys) { vy1 = btif->ys; }

	for (h = h0; h <= scr->top; h++) {
		sht = scr->shts[h];
		sid = sht - scr->shts0; /* 将进行了减法计算的地址作为图层号码使用 */
		buf = sht->buf;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) {bx0 = 0;}
		if (by0 < 0) {by0 = 0;}
		if (bx1 > sht->bxs) {bx1 = sht->bxs;}
		if (by1 > sht->bys) {by1 = sht->bys;}

		for (by = by0; by < by1; by++) {
			vy = sht->vy0 + by;
			for (bx = bx0; bx < bx1; bx++) {
				vx = sht->vx0 + bx;
				if (buf[by * sht->bxs + bx] != sht->cliv) {
					map[vy * btif->xs + vx] = sid;
				}
			}
		}
	}
}

void sheet_slide(struct sheet *sht, int vx0, int vy0) {
	int ox=sht->vx0, oy = sht->vy0;
	sht->vx0 = vx0;
	sht->vy0 = vy0;
	if (sht->height >= 0) {
		sctrler_refreshmap(ox, oy, ox + sht->bxs, oy + sht->bys, 0);
		sctrler_refreshmap(vx0, vy0, vx0 + sht->bxs, vy0 + sht->bys, sht->height);
		sctrler_refreshx(ox, oy, ox+sht->bxs, oy+sht->bys, 0, sht->height-1);
		sctrler_refreshx(vx0, vy0, vx0 + sht->bxs, vy0 + sht->bys, sht->height, sht->height);
	}
}

void sheet_free(struct sheet *sht) {
	if (sht->height >= 0) {
		sheet_setheight(sht, -1);
	}
	sht->flag = 0;
}

void sheet_put_str(struct sheet *xmain, int x, int y, int bg, int fg, char *text, int len) {
	draw_box(xmain->buf, xmain->bxs, bg, x, y, x + len * 8, y + 16);
	put_str(xmain->buf, xmain->bxs, x, y, fg, text);
	sheet_refresh(xmain, x, y, x + len * 8, y + 16);
}
