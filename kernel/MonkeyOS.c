#include "kernel/MonkeyOS.h"

struct fifo xmainfifobuf;
unsigned int memtotal;
struct mdec mouse_decoder;
// 其他指针
struct BootInfo *btif=(struct BootInfo*)0x0ff0;
struct task *task_mainloop;
char *_mscur;
// 控制器
struct tctrler *tcr;
struct taskctrler *tkcr;
struct mctrler *mcr;
struct sctrler *scr;
// 全局图层
struct sheet *sht_back;
struct sheet *sht_ms;
int mx=0, my=0;

static char KEYDATA_SHIFT[58] = { //按下shift
	0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0, 0, 'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', 0, 0, 0, 0
};

static char KEYDATA_UNSHIFT[58] = { //没按下shift
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0, 'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', 0, 0, 0, ' '
};

void MonkeyMain(void) {
	char *s;
	int i;

	// 直接用栈空间,因为主函数死循环
	unsigned int bbuf[512];
	init_fifo(&xmainfifobuf, 512, bbuf, 0);

	init_kernel();

	unsigned char p_shift=0; //按下shift?
	char *nowkeydata=KEYDATA_SHIFT;
	unsigned char p_capslock=0; //capslock打开(大写)

	//时钟测试
	struct timer *cur_timer; //光标定时器
	cur_timer = timer_alloc();
	timer_init(cur_timer, &xmainfifobuf, 0);
	timer_set(cur_timer, 50); //0.5s

	//显示相关
	char *testwinbuf;
	struct sheet *sht_tw;

	//窗口测试
	int wxs=120, wys=120;
	struct mwindow *tw = init_mwindow("xyz", wxs, wys);
	sht_tw = tw->sht;
	mwindow_draw(tw);
	struct mwindow_Label *tl = mwindow_Label_alloc(tw, 0, 16, 16, 7, "hhh");
	mwindow_Label_draw(tl);
	int twx=200, twy=200;
	sheet_slide(sht_tw, twx, twy);
	int cur_x=0;
	int nowcur_color=16;
	sheet_setheight(sht_tw, 1);

	for (;;) {
		io_cli();
		if (fifo_sts(&xmainfifobuf) == 0) {
			task_sleep(task_mainloop);
			io_sti();
		} else {
			i = fifo_get(&xmainfifobuf);
			io_sti();
			if (256 <= i && i <= 511) {
				i -= K_DT0;
				sprintf(s, "in key: i %d", i);
				sheet_put_str(sht_back, 0, 48, 0, 7, s, 20);
				if (p_shift) {
					nowkeydata = KEYDATA_SHIFT;
				} else {
					nowkeydata = KEYDATA_UNSHIFT;
				}
				if (i < 58) {
					//简化后的键盘解码只有58
					if (nowkeydata[i] != 0 && cur_x < 8*17) {
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
					p_shift = 0;
				}
				if (i == 58) {
					//capslock
					p_capslock = (p_capslock + 1) % 2;
				}
				if (i == 83) {
					//del
					mouse_decoder.st++;
					if (mouse_decoder.st == 4) {
						mouse_decoder.st = 1;
					}
				}

				draw_box(sht_tw->buf, sht_tw->bxs, nowcur_color, cur_x, 16, cur_x + 8, 32);
				sheet_refresh(sht_tw, cur_x, 16, cur_x + 8, 32);
			} else if (512 <= i && i <= 767) {
				sprintf(s, "in ms: i %d", i);
				sheet_put_str(sht_back, 0, 64, 0, 7, s, 20);
				if (mdecode(&mouse_decoder, i - M_DT0) != 0) {
					//解析成功
					sprintf(s, "mouse:[lcr %d %d] m:%d", mouse_decoder.x, mouse_decoder.y, mouse_decoder.z);
					if ((mouse_decoder.btn & 0x01) != 0) {
						s[7] = 'L';
						mwindow_Label_setX(tl, 8);
					}
					if ((mouse_decoder.btn & 0x02) != 0) {
						s[9] = 'R';
						mwindow_Label_setX(tl, 2);
					}
					if ((mouse_decoder.btn & 0x04) != 0) {
						s[8] = 'C';
						mwindow_Label_setText(tl,  "adfsdaf");
					}
					sheet_put_str(sht_back, 24, 16, 0, 7, s, 30);
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
					sheet_put_str(sht_back, 0, 0, 0, 7, s, 20);
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
