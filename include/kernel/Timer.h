#ifndef TIMER_KERNEL_H
#define TIMER_KERNEL_H

#include "kernel/Functions.h"
#include "kernel/Memory.h"

#define TIMER_F_ALLOC	1
#define TIMER_F_USING	2
#define PIT_CTRL		0x0043
#define PIT_CNT0		0x0040
#define MAX_TIMERS		500

struct timer {
	struct timer *next;
	unsigned int timeout, flags;
	struct fifo *fifobuf;
	int data;
};

struct tctrler {
	unsigned int count, next;
	struct timer *t0;
	struct timer timers[MAX_TIMERS];
};

void init_pit(void);
struct timer *timer_alloc(void);
void timer_free(struct timer *xmain);
void timer_init(struct timer *xmain, struct fifo *fifobuf, int data);
void timer_set(struct timer *xmain, unsigned int timeout);

#endif
