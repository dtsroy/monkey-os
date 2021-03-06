#include "mkpack.h"

struct fifo k_if;
struct fifo m_if;

void init_k_m_if(int ks, int ms, char *kb, char *mb) {
	init_fifo(&k_if, ks, kb);
	init_fifo(&m_if, ms, mb);
}

void wait_kr(void) {
	for (;;) {
		if ((io_inp8(P_KEYSTA) & 0x02) == 0) {
			return;
		}
	}
}

void init_keyboard(void) {
	//初始化键盘电路
	wait_kr();
	io_outp8(P_KEYCMD, KEYCMD_WRITE_MODE);
	wait_kr();
	io_outp8(P_KEYDAT, KBC_MODE);
}

void init_mouse(void) {
	//不能叫init,但为了整齐,激活
	wait_kr();
	io_outp8(P_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_kr();
	io_outp8(P_KEYDAT, MOUSECMD_ENABLE);
}

int mdecode(struct mdec *xmain, unsigned char *dat) {
	switch (xmain->st) {
		case 0:
			if (dat == 0xfa) {
				xmain->st = 1;
			}
			return 0;
		case 1:
			xmain->buf[0] = dat;
			xmain->st = 2;
			return 0;
		case 2:
			xmain->buf[1] = dat;
			xmain->st = 3;
			return 0;
		case 3:
			xmain->buf[2] = dat;
			xmain->st = 1;
			xmain->btn = xmain->buf[0] & 0x07; //取后3位
			xmain->x = xmain->buf[1];
			xmain->y = xmain->buf[2];
			if ((xmain->buf[0] & 0x10) != 0) {
				xmain->x |= 0xffffff00;
			}
			if ((xmain->buf[0] & 0x20) != 0) {
				xmain->y |= 0xffffff00;
			}
			xmain->y = -xmain->y;
			return 1;
	}
	return -1; //不能到这里,但还是保障一下
}
