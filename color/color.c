/********************************************************************************
* @File name: color.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-12
* @Description: c语言编写的应用程序 用于显示色阶
********************************************************************************/

#include "apilib.h"

void Main () {
	char *buf;
	int win, x, y, r, g, b;
	apiInitMalloc();
	buf = apiMalloc(144 * 164);
	win = apiOpenWindow(buf, 144, 164, -1, "color");
	for (y = 0; y < 128; y++) {
		for (x = 0; x < 128; x++) {
			r = x * 2;
			g = y * 2;
			b = 0;
			buf[(x + 8) + (y + 28) * 144] = 16 + (r / 43) + (g / 43) * 6 + (b / 43) * 36;
		}
	}
	apiRefreshWin(win, 8, 28, 136, 156);
	apiGetKey(1);
	apiEnd();
}

