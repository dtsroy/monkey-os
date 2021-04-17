#include "mkpack.h"

struct fifo xmainfifobuf;
struct tctrler tcr;
struct sheet *sht_back;

void MonkeyMain(void) {
	struct BootInfo *btif = (struct BootInfo*) 0x0ff0;
	char *s;
	int i;
	unsigned int xmainfifobuf_buf[512];
	init_fifo(&xmainfifobuf, 512, xmainfifobuf_buf);

	//GDT IDT PIC 初始化
	init_gdtidt();
	init_pic();
	io_sti(); //初始化完成,放开cpu中断标志

	init_pit();
	io_outp8(PIC0_IMR, 0xf8); //放开键盘 && PIC1 PIT 11111000
	io_outp8(PIC1_IMR, 0xef); //放开鼠标 11101111
	
	//键鼠初始化
	struct mdec mouse_decoder;
	init_keyboard();
	init_mouse();
	mouse_decoder.st = 0;

	//时钟测试
	struct timer *t1;
	t1 = timer_alloc();
	timer_init(t1, 3);
	timer_set(t1, 300);

	struct timer *t2;
	t2 = timer_alloc();
	timer_init(t2, 5);
	timer_set(t2, 500);

	//内存管理初始化
	struct mctrler *mcr = (struct mctrler *)MCTRLER_ADDR;

	unsigned int memtotal = getmem(0x400000, 0xfffffffff);
	init_mctrler(mcr);
	mctrler_free(mcr, 0x1000, 0x9e000);
	mctrler_free(mcr, 0x400000, memtotal - 0x400000);

	//显示相关
	init_palette();
	char _mscur[12*12], *_backbuf, *testwinbuf;
	
	//图层相关
	struct sctrler *scr;
	struct sheet *sht_ms;
	struct sheet *sht_tw;
	int mx = (btif->xs - 12) / 2;
	int my = (btif->ys - 12 - 14) / 2;

	scr = init_sctrler(mcr, btif->vram, btif->xs, btif->ys);
	sht_back = sctrler_alloc(scr);
	sht_ms = sctrler_alloc(scr);
	_backbuf = (unsigned char *) mctrler_allocx(mcr, btif->xs * btif->ys);
	
	sheet_setbuf(sht_back, _backbuf, btif->xs, btif->ys, -1);
	sheet_setbuf(sht_ms, _mscur, 12, 12, 99);

	//屏幕初始化1次
	init_screen(_backbuf, btif->xs, btif->ys);
	init_pointer(_mscur, 99);

	sheet_slide(sht_back, 0, 0);
	
	sheet_slide(sht_ms, mx, my);
	sheet_setheight(sht_back, 0);
	sheet_setheight(sht_ms, 2);

	//窗口测试
	testwinbuf = mctrler_allocx(mcr, 300*300);
	sht_tw = sctrler_alloc(scr);
	struct mwindow *tw = init_mwindow("_test!", sht_tw, 300, 300);
	sheet_setbuf(sht_tw, testwinbuf, 300, 300, 6);
	mwindow_draw(tw);
	sheet_setheight(sht_tw, 1);
	sheet_slide(sht_tw, 200, 200);

	// struct textinfo x = packText("hhh", 0, 7, 3);
	// struct mwindow_Label *tl;
	// mwindow_Label_new(tl, tw, 0, 0, "hhh", 0, 7, 3);
	// mwindow_Label_draw(tl);
	sheet_put_str(tw->sht, 0, 16, 16, 7, "gfdgfs", 6);

	sprintf(s, "memory %dMB, free:%dkb", memtotal / (1024*1024), mctrler_total(mcr) / 1024);
	sheet_put_str(sht_back, 0, 32, 0, 7, s, 30);

	unsigned int count=0;
	for (;;) {
		count++;
		io_cli();
		if (fifo_sts(&xmainfifobuf) == 0) {
			io_sti();
		}else {
			i = fifo_get(&xmainfifobuf);
			// sprintf(s, "i is: %d", i);
			// if (i==3)
			// sheet_put_str(sht_back, 0, 48, 0, 7, s, 20);
			io_sti();
			if (256 <= i && i <= 511) {
				sprintf(s, "%02X", i - K_DT0);
				sheet_put_str(sht_back, 0, 16, 0, 7, s, 2);
			} else if (512 <= i && i <= 767) {

				if (mdecode(&mouse_decoder, i - M_DT0) != 0) {
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
					sheet_put_str(sht_back, 24, 16, 0, 7, s, 20);
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
					if (mx > btif->xs - 1) {
						mx = btif->xs - 1;
					}
					if (my > btif->ys - 1) {
						my = btif->ys - 1;
					}
					sprintf(s, "(%d, %d)", mx, my);
					draw_box(_backbuf, btif->xs, 0, 0, 0, 320, 16);
					put_str(_backbuf, btif->xs, 0, 0, 7, s);
					sheet_refresh(sht_back, 0, 0, 320, 16);
					// sheet_refresh(sht_ms, 0, 16, 80, 16);
					sheet_slide(sht_ms, mx, my);
				}
			} else if (i == 3) {
				count = 0;
				sheet_put_str(sht_back, 0, 112, 0, 7, "3s!", 3);
			} else if (i == 5) {
				sprintf(s, "%d", count);
				sheet_put_str(sht_tw, 0, 16, 16, 7, s, strlen(s));
			}
		}
	}
}

