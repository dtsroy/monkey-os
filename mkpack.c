#include "mkpack.h"

void MonkeyMain(void) {
	struct BootInfo *btif = (struct BootInfo*) 0x0ff0;
	init_palette();
	init_screen(btif->vram, btif->xs, btif->ys);
	init_gdtidt();
	init_pic();
	io_sti(); //初始化完成,放开cpu中断标志
	io_outp8(PIC0_IMR, 0xf9); //放开键盘
	io_outp8(PIC1_IMR, 0xef); //放开鼠标
	fin();
}

