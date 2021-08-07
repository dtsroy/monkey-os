#ifndef TASK_KERNEL_H
#define TASK_KERNEL_H

#include "kernel/Memory.h"
#include "kernel/Functions.h"
#include "kernel/SystemStructure.h"
#include "kernel/Timer.h"

#define MAX_TASKS		1000	// 最大任务数量
#define TASK_GDT0		3		// gdt 分配的初始值
#define MAX_TASKS_LV	100		// 每个level最多几个task
#define MAX_TASKLEVELS	10		// 最多几个level

struct tss {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};

struct task {
	int sel, flag; //sel->GDT
	int level, priority; //优先
	struct tss _tss;
};

struct task_level {
	int running, now; //正在运行多少个  当前运行哪个
	struct task *tasks[MAX_TASKS_LV];
};

struct taskctrler {
	int nl; //活动中level
	unsigned char clv; //下次是否更改level
	struct task_level levels[MAX_TASKLEVELS];
	struct task tk0[MAX_TASKS];
};
struct task *task_now(void);
void task_add(struct task *xmain);
void task_remove(struct task *xmain);
void task_switchx(void);
struct task *task_init(void);
struct task *task_alloc(void);
void task_run(struct task *xmain, int level, int priority);
void task_switch(void);
void task_sleep(struct task *xmain);

#endif
