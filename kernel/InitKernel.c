#include "kernel/InitKernel.h"

extern struct fifo xmainfifobuf;
extern unsigned int memtotal;
extern struct mdec mouse_decoder;
extern struct BootInfo *btif;
extern struct task *task_mainloop;
extern char *_mscur;
extern struct tctrler *tcr;
extern struct taskctrler *tkcr;
extern struct mctrler *mcr;
extern struct sctrler *scr;
extern struct sheet *sht_back;
extern struct sheet *sht_ms;
extern int mx, my;

void init_kernel(void) {
	//启动信息
	btif = (struct BootInfo*) 0x0ff0;

	//内存管理初始化
	mcr = (struct mctrler *)MCTRLER_ADDR;
	memtotal = getmem(0x400000, 0xffffffff);
	init_mctrler();
	mctrler_free((void*)0x1000, 0x9e000);
	mctrler_free((void*)0x400000, memtotal - 0x400000);

	//GDT IDT PIC 初始化
	init_gdtidt();
	init_pic();
	io_sti(); //初始化完成,放开cpu中断标志

	init_pit(); //初始化pit
	io_outp8(PIC0_IMR, 0xf8); //放开键盘 && PIC1 PIT 11111000
	io_outp8(PIC1_IMR, 0xef); //放开鼠标 11101111

	// 键鼠(因为缓冲区在主函数call这个函数前已经初始化)
	init_keyboard();
	init_mouse();
	mouse_decoder.st = 2;

	//图层管理初始化
	init_palette(); // 先初始化硬件
	init_sctrler();

	// 指针
	_mscur = mctrler_alloc(144);
	init_pointer(_mscur, 99);

	// 初始化图层
	sht_back = sctrler_alloc();
	sht_ms = sctrler_alloc();
	sheet_setbuf(sht_back, (unsigned char*)mctrler_alloc(btif->xs*btif->ys), btif->xs, btif->ys, -1);
	sheet_setbuf(sht_ms, _mscur, 12, 12, 99);
	sheet_slide(sht_back, 0, 0);
	sheet_slide(sht_ms, mx, my);
	sheet_setheight(sht_back, 0);
	sheet_setheight(sht_ms, 2);

	//主循环任务
	task_mainloop = task_init();
	xmainfifobuf.tk = task_mainloop;
	task_run(task_mainloop, 1, 0);

}