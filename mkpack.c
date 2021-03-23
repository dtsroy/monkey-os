#include "mkpack.h"

extern struct fifo k_if, m_if;

void MonkeyMain(void) {
	struct BootInfo *btif = (struct BootInfo*) 0x0ff0;
	char *s, keybuf[32], mousebuf[128], _mscur[12*12], *_backbuf;
	int i, mx, my;
	unsigned int memtotal;
	struct mdec mouse_decoder;
	struct sctrler *scr;
	struct sheet *sht_ms, *sht_back;
	mx = (btif->xs - 12) / 2;
	my = (btif->ys - 12 - 14) / 2;

	init_gdtidt();
	init_pic();
	io_sti(); //初始化完成,放开cpu中断标志

	init_k_m_if(32, 128, keybuf, mousebuf);
	io_outp8(PIC0_IMR, 0xf9); //放开键盘 && PIC1
	io_outp8(PIC1_IMR, 0xef); //放开鼠标

	init_keyboard();
	init_mouse();
	mouse_decoder.st = 0;

	struct mctrler *mcr = (struct mctrler *)MCTRLER_ADDR;

	memtotal = getmem(0x400000, 0xfffffffffffffffffffff);//0xffffffff
	init_mctrler(mcr);
	mctrler_free(mcr, 0x1000, 0x9e000);
	mctrler_free(mcr, 0x400000, memtotal - 0x400000);

	init_palette();
	

	scr = init_sctrler(mcr, btif->vram, btif->xs, btif->ys);
	sht_back = sctrler_alloc(scr);
	sht_ms = sctrler_alloc(scr);
	_backbuf = (unsigned char *) mctrler_allocx(mcr, btif->xs * btif->ys);
	
	sheet_setbuf(sht_back, _backbuf, btif->xs, btif->ys, -1);
	sheet_setbuf(sht_ms, _mscur, 12, 12, 99);

	init_screen(_backbuf, btif->xs, btif->ys);
	init_pointer(_mscur, 99);

	sctrler_slide(scr, sht_back, 0, 0);
	
	sctrler_slide(scr, sht_ms, mx, my);
	sctrler_setheight(scr, sht_back, 0);
	sctrler_setheight(scr, sht_ms, 1);

	sprintf(s, "memory %dMB, free:%dkb", memtotal / (1024*1024), mctrler_total(mcr) / 1024);
	put_str(_backbuf, btif->xs, 0, 32, 3, s);
	sctrler_refresh(scr, sht_back, 0, 0, btif->xs, btif->ys);
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
				draw_box(_backbuf, btif->xs, 10, 0, 16, 15, 31);
				put_str(_backbuf, btif->xs, 0, 16, 3, s);
				sctrler_refresh(scr, sht_back, 0, 16, 16, 32);
			}else if (ms != 0) {
				i = fifo_get(&m_if);
				io_sti();
				if (mdecode(&mouse_decoder, i) != 0) {
					//解析成功
					sprintf(s, "mouse:[lcr %d %d]", mouse_decoder.x, mouse_decoder.y);
					if ((mouse_decoder.btn & 0x01) != 0) {
						s[7] = 'L';
					}
					if ((mouse_decoder.btn & 0x02) != 0) {
						s[9] = 'R';
					}
					if ((mouse_decoder.btn & 0x04) != 0) {
						s[8] = 'C';
					}
					draw_box(_backbuf, btif->xs, 10, 33, 16, 320, 31);
					put_str(_backbuf, btif->xs, 32, 16, 3, s);
					sctrler_refresh(scr, sht_back, 32, 16, 320, 32);
					//隐藏鼠标
					//draw_box(_backbuf, btif->xs, 10, mx, my, mx+12, my+12);
					//重新计算mx,my
					mx += mouse_decoder.x;
					my += mouse_decoder.y;
					//防止越界
					if (mx < 0) {
						mx = 0;
					}
					if (my < 0) {
						my = 0;
					}
					if (mx > btif->xs - 12) {
						mx = btif->xs - 12;
					}
					if (my > btif->ys - 12) {
						my = btif->ys - 12;
					}
					sprintf(s, "(%d, %d)", mx, my);
					draw_box(_backbuf, btif->xs, 10, 0, 0, 320, 16);
					put_str(_backbuf, btif->xs, 3, 0, 0, s);
					sctrler_refresh(scr, sht_back, 0, 0, 320, 16);
					sctrler_refresh(scr, sht_ms, 0, 16, 80, 16);
					sctrler_slide(scr, sht_ms, mx, my);
				}
			}
		}
	}
}

