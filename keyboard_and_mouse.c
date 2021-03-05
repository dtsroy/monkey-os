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
