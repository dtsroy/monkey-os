#include "mkpack.h"

extern struct fifo xmainfifobuf;
extern struct tctrler tcr;
extern struct sheet *sht_back;
extern struct timer *task_timer;

void init_pic(void) {
	io_outp8(PIC0_IMR, 0xff); //主PIC屏蔽所有中断
	io_outp8(PIC1_IMR, 0xff); //从PIC屏蔽所有中断

	io_outp8(PIC0_ICW1, 0x11); //
	io_outp8(PIC0_ICW2, 0x20); //IQR0-7(主) -> INT 0x20-0x27
	io_outp8(PIC0_ICW3, 1<<2); //00000100, 从->主2
	io_outp8(PIC0_ICW4, 0x01); //

	io_outp8(PIC1_ICW1, 0x11); //
	io_outp8(PIC1_ICW2, 0x28); //IQR8-15(从) -> INT 0x28-0x2f
	io_outp8(PIC1_ICW3, 2); //从->IQR2
	io_outp8(PIC1_ICW4, 0x01); //

	io_outp8(PIC0_IMR, 0xfb); //主PIC除IQR2全部屏蔽
	io_outp8(PIC1_IMR, 0xff); //从继续屏蔽
}

void ihr21(int *esp) { //键盘中断
	unsigned char *dat;
	io_outp8(PIC0_OCW, 0x61); //IQR1受理完毕
	dat = io_inp8(P_KEYDAT);
	fifo_put(&xmainfifobuf, dat + K_DT0);
}

void ihr27(int *esp) { //别管这是啥了
	io_outp8(PIC0_OCW, 0x67); /* 通知PIC的IRQ-07（参考7-1） */
	return;
}

void ihr2c(int *esp) { //鼠标中断
	unsigned char dat;
	io_outp8(PIC1_OCW, 0x64);
	io_outp8(PIC0_OCW, 0x62); //受理完毕
	dat = io_inp8(P_KEYDAT);
	fifo_put(&xmainfifobuf, dat + M_DT0);
}

void ihr20(int *esp) {
	struct timer *timer;
	unsigned char ts=0;
	io_outp8(PIC0_OCW, 0x60); /* 把IRQ-00接收信号结束的信息通知给PIC */
	tcr.count++;
	if (tcr.next > tcr.count) {
		return;
	}
	timer = tcr.t0; /* 首先把最前面的地址赋给timer */
	for (;;) {
	/* 因为timers的定时器都处于运行状态，所以不确认flags */
		if (timer->timeout > tcr.count) {
			break;
		}
		/* 超时 */
		// sheet_put_str(sht_back, 0, 96, 0, 7, "timeout!!!", 10);
		timer->flags = TIMER_F_ALLOC;
		if (timer != task_timer) {
			fifo_put(timer->fifobuf, timer->data);
		} else {
			ts = 1;
		}
		timer = timer->next; /* 将下一个定时器的地址赋给timer*/
	}
	tcr.t0 = timer;
	tcr.next = timer->timeout;
	if (ts != 0) {
		task_switch();
	}
	return;
}
