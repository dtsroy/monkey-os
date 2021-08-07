#ifndef INTERRUPTION_KERNEL_H
#define INTERRUPTION_KERNEL_H

#include "kernel/Buffer.h"
#include "kernel/Timer.h"
#include "kernel/Functions.h"
#include "kernel/Task.h"
#include "kernel/Keyboard.h"
#include "kernel/Mouse.h"

#define PIC0_ICW1 0x0020
#define PIC0_OCW 0x0020
#define PIC0_IMR 0x0021
#define PIC0_ICW2 0x0021
#define PIC0_ICW3 0x0021
#define PIC0_ICW4 0x0021
#define PIC1_ICW1 0x00a0
#define PIC1_OCW 0x00a0
#define PIC1_IMR 0x00a1
#define PIC1_ICW2 0x00a1
#define PIC1_ICW3 0x00a1
#define PIC1_ICW4 0x00a1

void init_pic(void);
void ihr21(int *esp);
void ihr27(int *esp);
void ihr2c(int *esp);
void ihr20(int *esp);

#endif
