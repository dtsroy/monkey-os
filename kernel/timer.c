#include "kernel/Timer.h"

extern struct tctrler *tcr;

void init_pit(void) {
	int i;
	struct timer *t;
	io_outp8(PIT_CTRL, 0x34);
	io_outp8(PIT_CNT0, 0x9c);
	io_outp8(PIT_CNT0, 0x2e);
	tcr = mctrler_allocx(sizeof(struct tctrler));
	tcr->count = 0;
	for (i = 0; i < MAX_TIMERS; i++) {
		tcr->timers[i].flags = 0; /* 没有使用 */
	}
	t = timer_alloc(); /* 取得一个 */
	t->timeout = 0xffffffff;
	t->flags = TIMER_F_USING;
	t->next = 0; /* 末尾 */
	tcr->t0 = t; /* 因为现在只有哨兵，所以他就在最前面*/
	tcr->next = 0xffffffff; /* 因为只有哨兵，所以下一个超时时刻就是哨兵的时刻 */
	return;
}

struct timer *timer_alloc(void) {
	int i;
	for (i = 0; i < MAX_TIMERS; i++) {
		if (tcr->timers[i].flags == 0) {
			tcr->timers[i].flags = TIMER_F_ALLOC;
			return &tcr->timers[i];
		}
	}
	return 0; /* 没找到 */
}

void timer_free(struct timer *xmain) {
	xmain->flags = 0;
}

void timer_init(struct timer *xmain, struct fifo *fifobuf, int data) {
	xmain->data = data;
	xmain->fifobuf = fifobuf;
}

void timer_set(struct timer *xmain, unsigned int timeout) {
	struct timer *t, *s;
	xmain->timeout = timeout + tcr->count;
	xmain->flags = TIMER_F_USING;
	io_cli();
	t = tcr->t0;
	if (xmain->timeout <= t->timeout) {
	/* 插入最前面的情况 */
		tcr->t0 = xmain;
		xmain->next = t; /* 下面是设定t */
		tcr->next = xmain->timeout;
		io_sti();
		return;
	}
	for (;;) {
		s = t;
		t = t->next;
		if (xmain->timeout <= t->timeout) {
		/* 插入s和t之间的情况 */
			s->next = xmain; /* s下一个是timer */
			xmain->next = t; /* timer的下一个是t */
			io_sti();
			return;
		}
	}
}
