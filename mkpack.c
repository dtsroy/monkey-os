#include "mkpack.h"

void HariMain(void) {
	struct BootInfo *btif = (struct BootInfo*) 0x0ff0;
	init_palette();
	init_screen(btif->vram, btif->xs, btif->ys);
	init_gdtidt();
	init_pic();
	io_outp8(PIC0_IMR, 0xf9); /* 开放PIC1和键盘中断(11111001) */
	io_outp8(PIC1_IMR, 0xef); /* 开放鼠标中断(11101111) */
	fin();
}

