#ifndef KEYBOARD_KERNEL_H
#define KEYBOARD_KERNEL_H

#include "kernel/Functions.h"

#define P_KEYDAT			0x0060
#define P_KEYSTA			0x0064
#define P_KEYCMD			0x0064
#define KEYCMD_WRITE_MODE	0x60
#define KBC_MODE			0x47
#define K_DT0				256

void wait_kr(void);
void init_keyboard(void);

#endif
