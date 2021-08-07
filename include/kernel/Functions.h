#ifndef FUNCTIONS_KERNEL_H
#define FUNCTIONS_KERNEL_H

struct BootInfo {
	char cyls, leds, vmode, reserve;
	short xs, ys;
	char *vram;
};

void io_hlt(void);
void io_outp8(int port, int data);
void io_cli(void);
int io_load_eflags(void);
void io_save_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
void ihr21x(void);
void ihr27x(void);
void ihr2cx(void);
void ihr20x(void);
void io_sti(void);
void io_shlt(void);
int io_inp8(int port);
int load_cr0(void);
void save_cr0(int cr0);
int getmemx(int start, int end);
void load_tr(int tr);
void fin(void);
void jmpfar(int eip, int cs);

#endif
