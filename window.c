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
