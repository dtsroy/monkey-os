#include "mkpack.h"

extern struct tctrler tcr;

void init_pit(void) {
	int i;
	struct timer *t;
	io_outp8(PIT_CTRL, 0x34);
	io_outp8(PIT_CNT0, 0x9c);
	io_outp8(PIT_CNT0, 0x2e);
	tcr.count = 0;
	for (i = 0; i < MAX_TIMERS; i++) {
		tcr.timers[i].flags = 0; /* 没有使用 */
	}
	t = timer_alloc(); /* 取得一个 */
	t->timeout = 0xffffffff;
	t->flags = TIMER_F_USING;
	t->next = 0; /* 末尾 */
	tcr.t0 = t; /* 因为现在只有哨兵，所以他就在最前面*/
	tcr.next = 0xffffffff; /* 因为只有哨兵，所以下一个超时时刻就是哨兵的时刻 */
	return;
}

struct timer *timer_alloc(void) {
	int i;
	for (i = 0; i < MAX_TIMERS; i++) {
		if (tcr.timers[i].flags == 0) {
			tcr.timers[i].flags = TIMER_F_ALLOC;
			return &tcr.timers[i];
		}
	}
	return 0; /* 没找到 */
}

void timer_free(struct timer *timer) {
	timer->flags = 0; /* 未使用 */
	return;
}

void timer_init(struct timer *timer, struct fifo *_fifo, int data) {
	timer->fifobuf = _fifo;
	timer->data = data;
	return;
}

void timer_set(struct timer *_timer, unsigned int timeout) {
	int e;
	struct timer *t, *s;
	_timer->timeout = timeout + tcr.count;
	_timer->flags = TIMER_F_USING;
	e = io_load_eflags();
	io_cli();
	t = tcr.t0;
	if (_timer->timeout <= t->timeout) {
	/* 插入最前面的情况 */
		tcr.t0 = _timer;
		_timer->next = t; /* 下面是设定t */
		tcr.next = _timer->timeout;
		io_save_eflags(e);
		return;
	}
	for (;;) {
		s = t;
		t = t->next;
		if (_timer->timeout <= t->timeout) {
		/* 插入s和t之间的情况 */
			s->next = _timer; /* s下一个是timer */
			_timer->next = t; /* timer的下一个是t */
			io_save_eflags(e);
			return;
		}
	}
}
