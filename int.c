#include "mkpack.h"

extern struct fifo k_if, m_if;

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
	fifo_put(&k_if, dat);
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
	fifo_put(&m_if, dat);
}
