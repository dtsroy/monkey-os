#include "mkpack.h"

void init_fifo(struct fifo *xmain, int size, unsigned int *buf, struct task *tk) {
	xmain->sz = size;
	xmain->wp = 0;
	xmain->rp = 0;
	xmain->free = size;
	xmain->addr = buf;
	xmain->flag = 0;
	xmain->tk = tk;
}

int fifo_put(struct fifo *xmain, int dat) {
	if (xmain->free == 0) {
		//溢出了
		xmain->flag = 1;
		return -1;
	}
	xmain->addr[xmain->wp] = dat;
	xmain->wp++;
	if (xmain->wp == xmain->sz) {
		//指针归零
		xmain->wp = 0;
	}
	xmain->free--;
	if (xmain->tk != 0) {
		if (xmain->tk->flag != 2) {
			//休眠中需唤醒
			task_run(xmain->tk, -1, 0);
		}
	}
	return 0;
}

int fifo_get(struct fifo *xmain) {
	int retd;
	if (xmain->free == xmain->sz) {
		//空闲==整体
		return -1;
	}
	retd = xmain->addr[xmain->rp];
	xmain->rp++;
	if (xmain->rp == xmain->sz) {
		//读指针归零
		xmain->rp = 0;
	}
	xmain->free++;
	return retd;
}

int fifo_sts(struct fifo *xmain) {
	return xmain->sz - xmain->free;
}
