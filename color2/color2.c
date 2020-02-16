/********************************************************************************
* @File name: color2.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-11
* @Description: c语言编写的应用程序 用6级色阶模拟21级色阶
********************************************************************************/

#include "apilib.h"

unsigned char rgb2Pal(int r, int g, int b, int x, int y) {
	static int table[4] = {3, 1, 0, 2};
	int i;
	// 判断奇偶
	x &= 1;
	y &= 1;
	i = table[x + y * 2];
	r = (r * 21) / 256;
	g = (g * 21) / 256;
	b = (b * 21) / 256;
	r = (r + i) / 4;
	g = (g + i) / 4;
	b = (b + i) / 4;
	return 16 + r + g * 6 + b * 36;
}

void Main () {
	char *buf;
	int win, x, y;
	apiInitMalloc();
	buf = apiMalloc(144 * 164);
	win = apiOpenWindow(buf, 144, 164, -1, "color");
	for (y = 0; y < 128; y++) {
		for (x = 0; x < 128; x++) {
			buf[(x + 8) + (y + 28) * 144] = rgb2Pal(x * 2, y * 2, 0, x, y);
		}
	}
	apiRefreshWin(win, 8, 28, 136, 156);
	apiGetKey(1);
	apiEnd();
}

