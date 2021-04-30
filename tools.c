#include "mkpack.h"

void fin(void) {
	for (;;) {
		io_hlt();
	}
}

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

