#ifndef WINDOW_KERNEL_H
#define WINDOW_KERNEL_H

#include "kernel/Memory.h"
#include "kernel/Sheet.h"
#include "kernel/Graphic.h"
#include "kernel/MonkeyStdlib.h"

struct mwindow {
	char *title;
	struct sheet *sht;
	int xs, ys;
};

struct mwindow *init_mwindow(char *title, int xs, int ys);
void mwindow_draw(struct mwindow *xmain);

struct mwindow_Label {
	struct mwindow *mw;
	int x, y;
	unsigned char bg, fg;
	char *text;
};

struct mwindow_Label *mwindow_Label_alloc(struct mwindow *mw, int x, int y, unsigned char bg, unsigned char fg, char *text);
void mwindow_Label_draw(struct mwindow_Label *xmain);
void mwindow_Label_hide(struct mwindow_Label *xmain);
void mwindow_Label_setX(struct mwindow_Label *xmain, int x);
void mwindow_Label_setY(struct mwindow_Label *xmain, int y);
void mwindow_Label_setText(struct mwindow_Label *xmain, char *text);

#endif
