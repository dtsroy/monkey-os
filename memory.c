#include "mkpack.h"

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
