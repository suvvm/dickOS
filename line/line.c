/********************************************************************************
* @File name: line.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-10
* @Description: c语言编写的应用程序，使用malloc申请内存空间，显示窗口随机绘制多条直线 手动刷新图层
********************************************************************************/

#include "apilib.h"

void Main () {
	char *buf;
	int win, i;
	apiInitMalloc();
	buf = apiMalloc(160 * 100);
	win = apiOpenWindow(buf, 160, 100, -1, "line");
	for (i = 0; i < 8; i++) {
		apiLineWin(win + 1, 8, 26, 77, i * 9 + 26, i);
		apiLineWin(win + 1, 88, 26, i * 9 + 88, 89, i);
	}
	apiRefreshWin(win, 6, 26, 154, 90);	// 手动刷新图层
	for (;;) {
		if (apiGetKey(1) == 0x0a) {
			break;
		}
	}
	apiEnd();
}
