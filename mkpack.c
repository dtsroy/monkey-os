void io_hlt(void);

void HariMain(void) {
	int i;
	char *p=0xa0000;
	for (i=0;i<=0x0ffff;i++) {
		p[i] = i&0x0f;
	}
}