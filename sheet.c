#include "mkpack.h"

struct sctrler *init_sctrler(struct mctrler *xmain, unsigned int vram, int xs, int ys) {
	struct sctrler *ret;
	int i;
	ret = (struct sctrler *) mctrler_allocx(xmain, sizeof(struct sctrler));
	if (ret == 0) {return 0;}
	ret->vram = vram;
	ret->xs = xs;
	ret->ys = ys;
	ret->top = -1;
	for (i=0; i<MAX_SHEETS; i++) {
		ret->shts0[i].flag = 0;
	}
	return ret;
}

struct sheet *sctrler_alloc(struct sctrler *xmain) {
	struct sheet ret;
	int i;
	for (i=0; i<MAX_SHEETS; i++) {
		if (xmain->shts0[i].flag == 0) {
			ret = xmain->shts0[i];
			ret.flag = 1;
			ret.height = -1;
			return &ret;
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

void sctrler_setheight(struct sctrler *xmain, struct sheet *sht, int height) {
	int tmp, old = sht->height;
	//调整高度
	if (height > xmain->top + 1) {
		height = xmain->top + 1;
	}
	if (height < -1) {
		height = -1;
	}
	//设定
	sht->height = height;
	//针对指针排列
	if (height > old) {
		//比以前低
		if (height >= 0) {
			//得到显示机会
			for (tmp=old; tmp>height; tmp--) {
				//old及以后后移
				xmain->shts[tmp] = xmain->shts[tmp - 1];
				xmain->shts[tmp]->height = tmp;
			}
			xmain->shts[height] = sht;
		} else {
			if (xmain->top > old) {
				//去掉old,再前移
				for (tmp=old; tmp<xmain->top; tmp++) {
					xmain->shts[tmp] = xmain->shts[tmp+1];
					xmain->shts[tmp]->height = tmp;
				}
			}
			//显示中被删了一个,top-1
			xmain->top--;
		}
		sctrler_refreshx(xmain, sht->vx0, sht->vy0, sht->vx0 + sht->bxs, sht->vy0 + sht->bys);
	} else if (height > old) {
		//比以前高
		if (old >= 0) {
			//本来就在显示
			for (tmp=old; tmp<height; tmp++) {
				xmain->shts[tmp] = xmain->shts[tmp + 1];
				xmain->shts[tmp]->height = tmp;
			}
			xmain->shts[height] = sht;
		} else {
			//本来为显示
			for (tmp=xmain->top; tmp>=height; tmp--) {
				xmain->shts[tmp + 1] = xmain->shts[tmp];
				xmain->shts[tmp + 1]->height = tmp + 1;
			}
			xmain->shts[height] = sht;
			xmain->top++;
		}
		sctrler_refreshx(xmain, sht->vx0, sht->vy0, sht->vx0 + sht->bxs, sht->vy0 + sht->bys);
	}
}

void sctrler_refresh(struct sctrler *xmain, struct sheet *sht, int bx0, int by0, int bx1, int by1) {
	if (sht->height >= 0) {
		//显示中
		sctrler_refreshx(xmain, sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1);
	}
}

void sctrler_refreshx(struct sctrler *xmain, int vx0, int vy0, int vx1, int vy1) {
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned char *buf, c, *vram=xmain->vram;
	struct sheet *sht;
	for (h=0; h<=xmain->top; h++) {
		sht = xmain->shts[h];
		buf = sht->buf;
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
				if (c != sht->cliv) {
					vram[vy * xmain->xs + vx] = c;
				}
			}
		}
	}
}

void sctrler_slide(struct sctrler *xmain, struct sheet *sht, int vx0, int vy0) {
	int ox=sht->vx0, oy = sht->vy0;
	sht->vx0 = vx0;
	sht->vy0 = vy0;
	if (sht->height >= 0) {
		sctrler_refreshx(xmain, ox, oy, ox+sht->bxs, oy+sht->bys);
		sctrler_refreshx(xmain, vx0, vy0, vx0 + sht->bxs, vy0 + sht->bys);
	}
}
