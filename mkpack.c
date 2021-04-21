#include "mkpack.h"

struct fifo xmainfifobuf;
struct tctrler tcr;
struct sheet *sht_back;
struct taskctrler *tkcr;
struct mctrler *mcr;
struct sctrler *scr;

struct sheet *sht_win_b;
struct sheet *sht_win_b2;

void task_b_main(void);
void task_b_main2(void);

static char KEYDATA_SHIFT[84] = { //按下shift
	0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0, 0, 'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0
};

static char KEYDATA_UNSHIFT[84] = { //没按下shift
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0, 'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', 0, 0, 0, ' ', 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0
};

void MonkeyMain(void) {
	struct BootInfo *btif = (struct BootInfo*) 0x0ff0;
	char *s;
	int i;
	unsigned int xmainfifobuf_buf[512];
	init_fifo(&xmainfifobuf, 512, xmainfifobuf_buf, 0);

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
	unsigned char p_shift=0; //按下shift?
	char *nowkeydata=KEYDATA_SHIFT;
	unsigned char p_capslock=0; //capslock打开(大写)

	//内存管理初始化
	mcr = (struct mctrler *)MCTRLER_ADDR;

	unsigned int memtotal = getmem(0x400000, 0xfffffffff);
	init_mctrler();
	mctrler_free(0x1000, 0x9e000);
	mctrler_free(0x400000, memtotal - 0x400000);

	//多任务临时测试
	struct task *tka, *tkbl[3];
	tka = task_init();
	xmainfifobuf.tk = tka;
	task_run(tka, 1, 0);

	//时钟测试
	struct timer *cur_timer; //光标定时器
	cur_timer = timer_alloc();
	timer_init(cur_timer, &xmainfifobuf, 0);
	timer_set(cur_timer, 50); //0.5s

	//显示相关
	init_palette();
	char _mscur[12*12], *_backbuf, *testwinbuf;
	
	//图层相关
	
	struct sheet *sht_ms;
	struct sheet *sht_tw;
	int mx = (btif->xs - 12) / 2;
	int my = (btif->ys - 12 - 14) / 2;

	scr = init_sctrler(btif->vram, btif->xs, btif->ys);

	sht_back = sctrler_alloc(scr);
	sht_ms = sctrler_alloc(scr);
	_backbuf = (unsigned char *) mctrler_allocx(btif->xs * btif->ys);
	
	sheet_setbuf(sht_back, _backbuf, btif->xs, btif->ys, -1);
	sheet_setbuf(sht_ms, _mscur, 12, 12, 99);

	//屏幕初始化1次
	init_screen(_backbuf, btif->xs, btif->ys);
	init_pointer(_mscur, 99);

	sheet_slide(sht_back, 0, 0);
	
	sheet_slide(sht_ms, mx, my);
	// sheet_setheight(sht_back, 0);
	// sheet_setheight(sht_ms, 2);

	//窗口测试
	int wxs=144, wys=32;
	testwinbuf = mctrler_allocx(wxs*wys);
	sht_tw = sctrler_alloc(scr);
	
	struct mwindow *tw = init_mwindow("_test!`~'", sht_tw, wxs, wys);
	sheet_setbuf(sht_tw, testwinbuf, wxs, wys, 6);
	mwindow_draw(tw);
	// sheet_setheight(sht_tw, 1);
	int twx=200, twy=200;
	sheet_slide(sht_tw, twx, twy);

	int cur_x=0;
	int nowcur_color=16;
	

	struct task *task_b;
	unsigned char *buf_b;
	struct mwindow *twwb;

	struct task *task_b2;
	unsigned char *buf_b2;
	struct mwindow *twwb2;

	sht_win_b = sctrler_alloc(scr);
	twwb = init_mwindow("yb1", sht_win_b, 144, 32);
	buf_b = mctrler_allocx(144*32);
	sheet_setbuf(sht_win_b, buf_b, 144, 32, -1);
	mwindow_draw(twwb);
	task_b = task_alloc();
	task_b->_tss.esp = mctrler_allocx(64 * 1024) + 64 * 1024 - 8;
	task_b->_tss.eip = (int) &task_b_main;
	task_b->_tss.es = 1 * 8;
	task_b->_tss.cs = 2 * 8;
	task_b->_tss.ss = 1 * 8;
	task_b->_tss.ds = 1 * 8;
	task_b->_tss.fs = 1 * 8;
	task_b->_tss.gs = 1 * 8;
	// *((int *) (task_b->_tss.esp + 4)) = i;
	task_run(task_b, 2, 1);

	sht_win_b2 = sctrler_alloc(scr);
	twwb2 = init_mwindow("yb2", sht_win_b2, 144, 32);
	buf_b2 = mctrler_allocx(144*32);
	sheet_setbuf(sht_win_b2, buf_b2, 144, 32, -1);
	mwindow_draw(twwb2);
	task_b2 = task_alloc();
	task_b2->_tss.esp = mctrler_allocx(64 * 1024) + 64 * 1024 - 8;
	task_b2->_tss.eip = (int) &task_b_main2;
	task_b2->_tss.es = 1 * 8;
	task_b2->_tss.cs = 2 * 8;
	task_b2->_tss.ss = 1 * 8;
	task_b2->_tss.ds = 1 * 8;
	task_b2->_tss.fs = 1 * 8;
	task_b2->_tss.gs = 1 * 8;
	// *((int *) (task_b->_tss.esp + 4)) = i;
	task_run(task_b2, 2, 4);

	sheet_setheight(sht_back, 0);

	sheet_slide(sht_win_b, 0, 123);
	sheet_setheight(sht_win_b, 1);

	sheet_slide(sht_win_b2, 0, 246);
	sheet_setheight(sht_win_b2, 2);

	sheet_setheight(sht_tw, 4);
	sheet_setheight(sht_ms, 5);
	sprintf(s, "memory %dMB, free:%dkb", memtotal / (1024*1024), mctrler_total() / 1024);
	sheet_put_str(sht_back, 0, 32, 0, 7, s, 30);
	for (;;) {
		// count++;
		io_cli();
		if (fifo_sts(&xmainfifobuf) == 0) {
			task_sleep(tka);
			io_sti();
		} else {
			i = fifo_get(&xmainfifobuf);
			// if (i==3)
			// sheet_put_str(sht_back, 0, 48, 0, 7, s, 20);
			io_sti();
			if (256 <= i && i <= 511) {
				i -= K_DT0;
				// sprintf(s, "in key: i %d", i);
				// sheet_put_str(sht_back, 0, 16, 0, 7, s, 20);
				if (p_shift) {
					nowkeydata = KEYDATA_SHIFT;
				} else {
					nowkeydata = KEYDATA_UNSHIFT;
				}
				if (i < 84) {
					if (nowkeydata[i] != 0 && cur_x < 8*18) {
						//最多18字符
						s[0] = nowkeydata[i];
						if ((p_capslock + p_shift) % 2 == 0) {
							//小写模式
							if (p_shift) {
								//按下shift,同时capslock打开
								if ('A' <= s[0] && s[0] <= 'Z') {
									s[0] += 0x20;
								}
							}
						} else {
							//大写
							if (!p_shift) {
								if ('a' <= s[0] && s[0] <= 'z') {
									s[0] -= 0x20;
								}
							}
						}
						s[1] = 0;//字符串结尾
						sheet_put_str(sht_tw, cur_x, 16, 16, 7, s, 1);
						cur_x += 8;
					}
				}
				if (i == 14 && cur_x > 0) {
					//退格
					sheet_put_str(sht_tw, cur_x, 16, 16, 7, " ", 1);
					cur_x -= 8;
				}
				if (i == 54 || i == 42) {
					//shift
					p_shift = 1;
				}
				if (i == 170 || i == 182) {
					//松开shift
					// sheet_put_str(sht_back, 0, 96, 16, 7, "shift!", 10);
					p_shift = 0;
				}
				if (i == 58) {
					//capslock
					p_capslock = (p_capslock + 1) % 2;
					// _shutdown();
				}

				draw_box(sht_tw->buf, sht_tw->bxs, nowcur_color, cur_x, 16, cur_x + 8, 32);
				sheet_refresh(sht_tw, cur_x, 16, cur_x + 8, 32);
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
				if (mouse_decoder.btn & 0x01 != 0) {
					sheet_slide(sht_tw, mx, my);
				}
			} else if (i <= 1) {
				if (i == 1) {
					timer_init(cur_timer, &xmainfifobuf, 0);
					nowcur_color = 7;
				} else {
					timer_init(cur_timer, &xmainfifobuf, 1);
					nowcur_color = 16;
				}
				timer_set(cur_timer, 50); //重置
				draw_box(sht_tw->buf, sht_tw->bxs, nowcur_color, cur_x, 16, cur_x + 8, 32);
				sheet_refresh(sht_tw, cur_x, 16, cur_x + 8, 32);
			}
		}
	}
}

void task_b_main(void)//struct sheet *sht_win_b
{
	struct fifo _fifo;
	struct timer *timer_1s;
	int i, count = 0, count0 = 0;
	char s[12];
	unsigned int fifobuf[128];
	// struct sheet *sht_win_b = 0xaa0000;
	sheet_put_str(sht_win_b, 0, 16, 0, 7, "hh", 2);
	init_fifo(&_fifo, 128, fifobuf, 0);
	timer_1s = timer_alloc();
	timer_init(timer_1s, &_fifo, 100);
	timer_set(timer_1s, 100);
	
	for (;;) {
		count++;
		io_cli();
		if (fifo_sts(&_fifo) == 0) {
			io_sti();
		} else {
			i = fifo_get(&_fifo);
			io_sti();
			if (i == 100) {
				sprintf(s, "%11d", count - count0);
				sheet_put_str(sht_win_b, 0, 16, 0, 7, s, 11);
				count0 = count;
				timer_set(timer_1s, 100);
			}
		}
	}
}

void task_b_main2(void)//struct sheet *sht_win_b
{
	struct fifo _fifo;
	struct timer *timer_1s;
	int i, count = 0, count0 = 0;
	char s[12];
	unsigned int fifobuf[128];
	// struct sheet *sht_win_b = 0xaa0000;
	sheet_put_str(sht_win_b2, 0, 16, 0, 7, "hh", 2);
	init_fifo(&_fifo, 128, fifobuf, 0);
	timer_1s = timer_alloc();
	timer_init(timer_1s, &_fifo, 100);
	timer_set(timer_1s, 100);
	
	for (;;) {
		count++;
		io_cli();
		if (fifo_sts(&_fifo) == 0) {
			io_sti();
		} else {
			i = fifo_get(&_fifo);
			io_sti();
			if (i == 100) {
				sprintf(s, "%11d", count - count0);
				sheet_put_str(sht_win_b2, 0, 16, 0, 7, s, 11);
				count0 = count;
				timer_set(timer_1s, 100);
			}
		}
	}
}

