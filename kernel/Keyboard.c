#include "kernel/Keyboard.h"

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
