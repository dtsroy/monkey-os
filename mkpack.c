#include "mkpack.h"

void HariMain(void) {
	struct BootInfo *btif = (struct BootInfo*) 0x0ff0;
	init_palette();
	init_screen(btif->vram, btif->xs, btif->ys);
	init_gdtidt();
	fin();
}

