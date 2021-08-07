#include "mkpack.h"

extern struct mctrler *mcr;

unsigned char test486(void) {
	char is486 = 0;
	unsigned int ef;
	//确认cpu386还是486以上
	ef = io_load_eflags();
	ef |= EFLAGS_AC_BIT; //AC标志位设为1
	io_save_eflags(ef);
	ef = io_load_eflags();

	if ((ef & EFLAGS_AC_BIT) != 0) {
		is486 = 1; //ac不为0,cpu有ac标志位
	}

	ef &= ~EFLAGS_AC_BIT; //退回ac位
	io_save_eflags(ef);
	return is486;
}

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

void init_mctrler() {
	mcr->frees = 0;
	mcr->maxfrees = 0;
	mcr->losts = 0;
	mcr->lostsize = 0;
}

unsigned int mctrler_total() {
	unsigned int ret = 0, it; //总计free
	for (it = 0; it<mcr->frees; it++) {
		ret += mcr->free[it].size;
	}
	return ret;
}

unsigned int mctrler_alloc(unsigned int size) {
	unsigned int it, ret;
	for (it = 0; it<mcr->frees; it++) {
		if (mcr->free[it].size >= size) {
			ret = mcr->free[it].addr;
			mcr->free[it].addr += size; //起始地址后移
			mcr->free[it].size -= size;
			if (mcr->free[it].size == 0) {
				//已经保证>=0
				mcr->frees--;
				for (; it<mcr->frees; it++) {
					mcr->free[it] = mcr->free[it+1]; //整体偏移
				}
			}
			return ret;
		}
	}
	return 0;
}

int mctrler_free(unsigned int addr, unsigned int size) {
	int i, j;
	//决定插入索引
	for (i=0; i<mcr->frees; i++) {
		if (mcr->free[i].addr > addr) {
			break;
		}
	}
	//i-1 的addr < addr < i 的addr
	if (i > 0) {
		//前面有可用
		if (mcr->free[i-1].addr + mcr->free[i-1].size == addr) {
			//与前面归纳
			mcr->free[i-1].size += size;
			if (i<mcr->frees) {
				if (addr + size == mcr->free[i].addr) {
					//与后面归纳
					mcr->free[i-1].size += mcr->free[i].size;
					mcr->frees--;
					//整体位移
					for (; i<mcr->frees; i++) {
						mcr->free[i] = mcr->free[i+1];
					}
				}
			}
			return 0;
		}
	}
	if (i < mcr->frees) {
		//不是最后一个
		if (addr + size == mcr->free[i].addr) {
			//与后面归纳
			mcr->free[i].addr = addr;
			mcr->free[i].size += size;
			return 0;
		}
	}
	if (mcr->frees < MCTRLER_MAX_FREES) {
		for (j = mcr->frees; j>i; j--) {
			// 整体后移
			mcr->free[i] = mcr->free[j-1];
		}
		mcr->frees++;
		if (mcr->frees > mcr->maxfrees) {
			//更新最大值
			mcr->maxfrees = mcr->frees;
		}
		mcr->free[i].addr = addr;
		mcr->free[i].size = size;
		return 0;
	}
	//还是不行,舍去
	mcr->losts++;
	mcr->lostsize += size;
	return -1;
}

unsigned int mctrler_allocx(unsigned int size) {
	unsigned int ret;
	size = (size + 0xfff) & 0xfffff000;
	ret = mctrler_alloc(size);
	return ret;
}

int mctrler_freex(unsigned int addr, unsigned int size) {
	int ret;
	size = (size + 0xfff) & 0xfffff000;
	ret = mctrler_free(addr, size);
	return ret;
}
