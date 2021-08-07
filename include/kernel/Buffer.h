#ifndef BUFFER_KERNEL_H
#define BUFFER_KERNEL_H

#include "kernel/Task.h"

struct fifo {
	int *addr; //缓冲区地址
	int wp, rp, sz, free, flag; //写入指针,读取指针,大小,空余,是否溢出(0, -1)
	struct task *tk;
};

void init_fifo(struct fifo *xmain, int size, unsigned int *buf, struct task *tk);
int fifo_put(struct fifo *xmain, int dat);
int fifo_get(struct fifo *xmain);
int fifo_sts(struct fifo *xmain);

#endif
