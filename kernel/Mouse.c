#include "kernel/Mouse.h"

void init_mouse(void) {
	//不能叫init,但为了整齐,激活
	wait_kr();
	io_outp8(P_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_kr();
	io_outp8(P_KEYDAT, MOUSECMD_ENABLE);

	//下面激活滚轮
	wait_kr();
	io_outp8(P_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_kr();
	io_outp8(P_KEYDAT, 0xf3); //设置采样率命令
	wait_kr();
	io_outp8(P_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_kr();
	io_outp8(P_KEYDAT, 200); //魔术序列:0
	wait_kr();
	io_outp8(P_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_kr();
	io_outp8(P_KEYDAT, 0xf3); //设置采样率命令
	wait_kr();
	io_outp8(P_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_kr();
	io_outp8(P_KEYDAT, 100); //魔术序列:1
	wait_kr();
	io_outp8(P_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_kr();
	io_outp8(P_KEYDAT, 0xf3); //设置采样率命令
	wait_kr();
	io_outp8(P_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_kr();
	io_outp8(P_KEYDAT, 80); //魔术序列:2
}

int mdecode(struct mdec *xmain, int dat) {
	char *s;
	switch (xmain->st) {
	case 0:
		xmain->st = 1; //添加滚轮后防止数据错位
		return 0;
	case 1:
		xmain->buf[0] = dat;
		xmain->st = 2;
		return 0;
	case 2:
		xmain->buf[1] = dat;
		xmain->st = 3;
		return 0;
	case 3:
		xmain->buf[2] = dat;
		xmain->st = 1;
		xmain->btn = xmain->buf[0] & 0x07; //取后3位
		xmain->x = xmain->buf[1];
		xmain->y = xmain->buf[2];
		if ((xmain->buf[0] & 0x10) != 0) {
			xmain->x |= 0xffffff00;
		}
		if ((xmain->buf[0] & 0x20) != 0) {
			xmain->y |= 0xffffff00;
		}
		xmain->y = -xmain->y;
		xmain->st = 4;
		return 0;
	case 4:
		xmain->buf[3] = dat;
		switch (dat&0xf) {
		case 0x00:
			//无滚动
			xmain->z = 0;
			break;
		case 0x01:
			//向上
			xmain->z = 1;
			break;
		case 0x0f:
			//向下
			xmain->z = -1;
			break;
		}
		xmain->st = 1;
		return 1;
	}
}
