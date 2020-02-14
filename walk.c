/********************************************************************************
* @File name: walk.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-10
* @Description: c语言编写的应用程序 使用方向键或小键盘2468控制*符号移动 回车退出
********************************************************************************/

#include "apilib.h"

void Main() {
	char *buf;
	int win, keyVal, x, y;
	apiInitMalloc();
	buf = apiMalloc(160 * 100);
	win = apiOpenWindow(buf, 160, 100, -1, "walk");
	apiBoxFillWin(win, 4, 24, 155, 95, 0);	// 黑色
	x = 76;
	y = 56;
	apiPutStrWin(win, x, y, 3, 1, "*");
	for (;;) {
		keyVal = apiGetKey(1);
		apiPutStrWin(win, x, y, 0, 1, "*");
		if (keyVal == '4' && x > 4) {
			x -= 8;
		}
		if (keyVal == '6' && x < 148) {
			x += 8;
		}
		if (keyVal == '8' && y > 24) {
			y -= 8;
		}
		if (keyVal == '2' && y < 80) {
			y += 8;
		}
		if (keyVal == 0x0a) {	// 按下回车结束
			break;
		}
		apiPutStrWin(win, x, y, 3, 1, "*");
	}
	apiCloseWin(win);
	apiEnd();
}
