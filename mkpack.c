#include "mkpack.h"

extern struct fifo k_if, m_if;

void MonkeyMain(void) {
	struct BootInfo *btif = (struct BootInfo*) 0x0ff0;
	char *s, keybuf[32], mousebuf[128];
	int i;

	init_gdtidt();
	init_pic();
	io_sti(); //初始化完成,放开cpu中断标志

	init_k_m_if(32, 128, keybuf, mousebuf);
	io_outp8(PIC0_IMR, 0xf9); //放开键盘 && PIC1
	io_outp8(PIC1_IMR, 0xef); //放开鼠标

	init_keyboard();
	init_mouse();

	init_palette();
	init_screen(btif->vram, btif->xs, btif->ys);

	for (;;) {
		io_cli();
		int ks = fifo_sts(&k_if);
		int ms = fifo_sts(&m_if);
		if (ks + ms == 0) {
			io_shlt();
		}else {
			if (ks != 0) {
				i = fifo_get(&k_if);
				io_sti();
				sprintf(s, "%02X", i);
				draw_box(btif->vram, btif->xs, 7, 0, 16, 15, 31);
				put_str(btif->vram, btif->xs, 0, 16, 3, s);
			}else if (ms != 0) {
				i = fifo_get(&m_if);
				io_sti();
				sprintf(s, "%02X", i);
				draw_box(btif->vram, btif->xs, 7, 16, 16, 31, 31);
				put_str(btif->vram, btif->xs, 16, 16, 3, s);
			}
		}
	}
}

