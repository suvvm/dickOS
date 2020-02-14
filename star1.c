/********************************************************************************
* @File name: star1.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-10
* @Description: c语言编写的应用程序，使用malloc申请内存空间，显示窗口绘制点
********************************************************************************/

#include "apilib.h"

void Main() {
	char *buf;
	int win;
	apiInitMalloc();
	buf = apiMalloc(150 * 100);
	win = apiOpenWindow(buf, 150, 100, -1, "star1");
	apiBoxFillWin(win, 6, 26, 143, 93, 0);	// 黑色
	apiPoint(win, 75, 59, 3);	// 黄色
	for (;;) {
		if (apiGetKey(1) == 0x0a) {	// 按下回车结束
			break;
		}
	}
	apiCloseWin(win);
	apiEnd();
}
