#include "kernel/Task.h"

extern struct taskctrler *tkcr;
struct timer *task_timer;

struct task *task_now(void) {
	struct task_level *l = &tkcr->levels[tkcr->nl];
	return l->tasks[l->now];
}

void task_add(struct task *xmain) {
	struct task_level *l = &tkcr->levels[xmain->level];
	l->tasks[l->running] = xmain; //长度=末尾索引+1
	l->running++;
	xmain->flag = 2; //运行
}

void task_remove(struct task *xmain) {
	int index;
	struct task_level *l = &tkcr->levels[xmain->level];

	for (index=0; index<l->running; index++) {
		if (l->tasks[index] == xmain) {
			break;
		}
	}

	l->running--;
	if (index < l->now) {
		l->now--;
	}
	if (l->now >= l->running) {
		//修正now
		l->now = 0;
	}
	xmain->flag = 1; //睡眠

	for (; index < l->running; index++) {
		l->tasks[index] = l->tasks[index+1];
	}
}

void task_switchx(void) {
	int i; //当前最高活跃level
	for (i=0; i<MAX_TASKLEVELS; i++) {
		if (tkcr->levels[i].running > 0) {
			break;
		}
	}
	tkcr->nl = i;
	tkcr->clv = 0;
}

struct task *task_init(void) {
	int i;
	struct task *ret, *idle;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	tkcr = mctrler_allocx(sizeof(struct taskctrler));

	for (i=0; i<MAX_TASKS; i++) {
		tkcr->tk0[i].flag = 0;
		tkcr->tk0[i].sel = (TASK_GDT0 + i) * 8;
		set_segmdesc(gdt + TASK_GDT0 + i, 103, (int) &tkcr->tk0[i]._tss, AR_TSS32);
	}

	for (i=0; i<MAX_TASKLEVELS; i++) {
		tkcr->levels[i].running = 0;
		tkcr->levels[i].now = 0;
	}
	ret = task_alloc();
	ret->flag = 2;
	ret->priority = 2; //0.02s
	ret->level = 0;
	task_add(ret);
	task_switchx();
	load_tr(ret->sel);
	task_timer = timer_alloc();
	timer_set(task_timer, ret->priority);

	idle = task_alloc();
	idle->_tss.esp = mctrler_allocx(64*1024) + 64*1024;
	idle->_tss.eip = (int) &fin;
	idle->_tss.es = 8;
	idle->_tss.cs = 16;
	idle->_tss.ss = 8;
	idle->_tss.ds = 8;
	idle->_tss.fs = 8;
	idle->_tss.gs = 8;
	task_run(idle, MAX_TASKLEVELS-1, 1);

	return ret;
}

struct task *task_alloc(void) {
	int i;
	struct task *ret;
	for (i=0; i<MAX_TASKS; i++) {
		if (tkcr->tk0[i].flag == 0) {
			//找到
			ret = &tkcr->tk0[i];
			ret->flag = 1;
			ret->_tss.eflags = 0x00000202; /* IF = 1; */
			ret->_tss.eax = 0; /*这里先置为0*/
			ret->_tss.ecx = 0;
			ret->_tss.edx = 0;
			ret->_tss.ebx = 0;
			ret->_tss.ebp = 0;
			ret->_tss.esi = 0;
			ret->_tss.edi = 0;
			ret->_tss.es = 0;
			ret->_tss.ds = 0;
			ret->_tss.fs = 0;
			ret->_tss.gs = 0;
			ret->_tss.ldtr = 0;
			ret->_tss.iomap = 0x40000000;
			return ret;
		}
	}
	return 0;
}

void task_run(struct task *xmain, int level, int priority) {
	if (level < 0) {
		//不改level
		level = xmain->level;
	}
	if (priority > 0) {
		xmain->priority = priority;
	}
	if (xmain->flag == 2 && xmain->level != level) {
		//活动中改变level
		task_remove(xmain);
	}
	if (xmain->flag != 2) {
		//睡眠中,需唤醒
		xmain->level = level;
		task_add(xmain);
	}
	tkcr->clv = 1;
}

void task_switch(void) {
	struct task_level *l = &tkcr->levels[tkcr->nl];
	struct task *newt, *nowt = l->tasks[l->now];
	l->now++;
	if (l->now == l->running) {
		l->now = 0;
	}
	if (tkcr->clv) {
		task_switchx();
		l = &tkcr->levels[tkcr->nl];
	}
	newt = l->tasks[l->now];
	timer_set(task_timer, newt->priority);
	if (newt != nowt) {
		jmpfar(0, newt->sel);
	}
}

void task_sleep(struct task *xmain) {
	struct task *nowt;
	if (xmain->flag == 2) {
		//活动中
		nowt = task_now();
		task_remove(xmain);
		if (xmain == nowt) {
			task_switchx();
			nowt = task_now();
			jmpfar(0, nowt->sel);
		}
	}
}
