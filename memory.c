#include "mkpack.h"

unsigned int getmem(unsigned int start, unsigned int end) {
	char is486 = test486();
	unsigned int cr0, ret;

	if (is486 == 1) {
		cr0 = load_cr0();
		cr0 |= CR0_CACHE_DISABLE;
		save_cr0(cr0);
	}

	ret = getmemx(start, end);

	if (is486 == 1) {
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE;
		save_cr0(cr0);
	}

	return ret;
}

void init_mctrler(struct mctrler *xmain) {
	xmain->frees = 0;
	xmain->maxfrees = 0;
	xmain->losts = 0;
	xmain->lostsize = 0;
}

unsigned int mctrler_total(struct mctrler *xmain) {
	unsigned int ret = 0, it; //总计free
	for (it = 0; it<xmain->frees; it++) {
		ret += xmain->free[it].size;
	}
	return ret;
}

unsigned int mctrler_alloc(struct mctrler *xmain, unsigned int size) {
	unsigned int it, ret;
	for (it = 0; it<xmain->frees; it++) {
		if (xmain->free[it].size >= size) {
			ret = xmain->free[it].addr;
			xmain->free[it].addr += size; //起始地址后移
			xmain->free[it].size -= size;
			if (xmain->free[it].size == 0) {
				//已经保证>=0
				xmain->frees--;
				for (; it<xmain->frees; it++) {
					xmain->free[it] = xmain->free[it+1]; //整体偏移
				}
			}
			return ret;
		}
	}
	return 0;
}

int mctrler_free(struct mctrler *xmain, unsigned int addr, unsigned int size) {
	int i, j;
	//决定插入索引
	for (i=0; i<xmain->frees; i++) {
		if (xmain->free[i].addr > addr) {
			break;
		}
	}
	//i-1 的addr < addr < i 的addr
	if (i > 0) {
		//前面有可用
		if (xmain->free[i-1].addr + xmain->free[i-1].size == addr) {
			//与前面归纳
			xmain->free[i-1].size += size;
			if (i<xmain->frees) {
				if (addr + size == xmain->free[i].addr) {
					//与后面归纳
					xmain->free[i-1].size += xmain->free[i].size;
					xmain->frees--;
					//整体位移
					for (; i<xmain->frees; i++) {
						xmain->free[i] = xmain->free[i+1];
					}
				}
			}
			return 0;
		}
	}
	if (i < xmain->frees) {
		//不是最后一个
		if (addr + size == xmain->free[i].addr) {
			//与后面归纳
			xmain->free[i].addr = addr;
			xmain->free[i].size += size;
			return 0;
		}
	}
	if (xmain->frees < MCTRLER_MAX_FREES) {
		for (j = xmain->frees; j>i; j--) {
			// 整体后移
			xmain->free[i] = xmain->free[j-1];
		}
		xmain->frees++;
		if (xmain->frees > xmain->maxfrees) {
			//更新最大值
			xmain->maxfrees = xmain->frees;
		}
		xmain->free[i].addr = addr;
		xmain->free[i].size = size;
		return 0;
	}
	//还是不行,舍去
	xmain->losts++;
	xmain->lostsize += size;
	return -1;
}
