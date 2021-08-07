#include "mkpack.h"

struct mwindow *init_mwindow(char *title, int xs, int ys) {
	ys += 16;
	struct mwindow *xmain=mctrler_alloc(sizeof(struct mwindow));
	xmain->title = title;
	xmain->sht = sctrler_alloc();
	sheet_setbuf(xmain->sht, (unsigned char *)mctrler_alloc(xs * ys), xs, ys, -1);
	xmain->xs = xs;
	xmain->ys = ys;
	return xmain;
}

void mwindow_draw(struct mwindow *xmain) {
	static char closeb[9][12] = {
		"############",
		"##XX####XX##",
		"###XX##XX###",
		"####XXXX####",
		"#####XX#####",
		"####XXXX####",
		"###XX##XX###",
		"##XX####XX##",
		"############"
	};
	char color;
	int x, y;
	draw_box(xmain->sht->buf, xmain->xs, 16, 0, 0, xmain->xs, xmain->ys); //主体
	draw_box(xmain->sht->buf, xmain->xs, 15, 0, 0, xmain->xs, 16); //顶端栏
	put_str(xmain->sht->buf, xmain->xs, 17, 0, 7, xmain->title); //标题 给图标流出空间
	for (x=0; x<12; x++) {
		for (y=0; y<9; y++) {
			color = closeb[y][x];
			if (color == '#') {
				color = 15;
			} else {
				color = 7;
			}
			xmain->sht->buf[(y + 4) * xmain->xs + (xmain->xs - 16 + x)] = color;
		}
	}
}

struct mwindow_Label *mwindow_Label_alloc(struct mwindow *mw, int x, int y, unsigned char bg, unsigned char fg, char *text) {
	//创建一个标签
	struct mwindow_Label *ret = mctrler_alloc(sizeof(struct mwindow_Label));
	ret->mw = mw;
	ret->x = x;
	ret->y = y;
	ret->bg = bg;
	ret->fg = fg;
	ret->text = text;
	return ret;
}

void mwindow_Label_draw(struct mwindow_Label *xmain) {
	//绘制标签
	sheet_put_str(xmain->mw->sht, xmain->x, xmain->y+16, xmain->bg, xmain->fg, xmain->text, strlen(xmain->text));
}

void mwindow_Label_hide(struct mwindow_Label *xmain) {
	//隐藏标签
	struct mwindow *mw = xmain->mw;
	draw_box(mw->sht->buf, mw->xs, 16, xmain->x, xmain->y+16, xmain->x+strlen(xmain->text)*8, xmain->y+32);
	sheet_refresh(mw->sht, xmain->x, xmain->y+16, xmain->x+strlen(xmain->text)*8, xmain->y+32);
}

void mwindow_Label_setX(struct mwindow_Label *xmain, int x) {
	//修改标签x坐标
	mwindow_Label_hide(xmain);
	xmain->x = x;
	mwindow_Label_draw(xmain);
}

void mwindow_Label_setY(struct mwindow_Label *xmain, int y) {
	//修改标签x坐标
	mwindow_Label_hide(xmain);
	xmain->y = y;
	mwindow_Label_draw(xmain);
}

void mwindow_Label_setText(struct mwindow_Label *xmain, char *text) {
	//修改标签x坐标
	mwindow_Label_hide(xmain);
	xmain->text = text;
	mwindow_Label_draw(xmain);
}
