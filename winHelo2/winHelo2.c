/********************************************************************************
* @File name: winHelo2.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-09
* @Description: c语言编写的应用程序用于显示窗口并显示字符绘制矩形
********************************************************************************/

#include "apilib.h"

void Main() {
	int win;
	char buf[150 * 50];
	win = apiOpenWindow(buf, 150, 50, -1, "hello");
	apiBoxFillWin(win, 8, 36, 141, 43, 3);	// 黄色
	apiPutStrWin(win, 28, 28, 0, 12, "hello, world");
	for (;;) {
		if (apiGetKey(1) == 0x0a) {
			break;
		}
	}
	apiEnd();
}
