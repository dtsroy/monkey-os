#ifndef MEMORY_KERNEL_H
#define MEMORY_KERNEL_H

#include "kernel/Functions.h"

#define EFLAGS_AC_BIT		0x00040000
#define MCTRLER_MAX_FREES	4096
#define MCTRLER_ADDR		0x3c0000 //内存控制地址
#define CR0_CACHE_DISABLE	0x60000000

struct freeif {
	unsigned addr, size;
};

struct mctrler {
	int frees, maxfrees, losts, lostsize;
	struct freeif free[MCTRLER_MAX_FREES];
};

unsigned int getmem(unsigned int start, unsigned int end);
void fin(void);
unsigned char test486(void);
void init_mctrler(void);
unsigned int mctrler_total(void);
void *mctrler_alloc(unsigned int size);
int mctrler_free(void *_addr, unsigned int size);
void *mctrler_allocx(unsigned int size);

#endif
