#ifndef MOUSE_KERNEL_H
#define MOUSE_KERNEL_H

#include "kernel/Functions.h"
#include "kernel/Sheet.h"
#include "kernel/Keyboard.h"

#define KEYCMD_SENDTO_MOUSE	0xd4
#define MOUSECMD_ENABLE		0xf4
#define M_DT0 512

struct mdec {
	unsigned char buf[4], st;
	int x, y, btn;
	signed int z;
};

void init_mouse(void);
int mdecode(struct mdec *xmain, unsigned char *dat);

#endif
