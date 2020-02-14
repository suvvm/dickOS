/********************************************************************************
* @File name: stars.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-10
* @Description: c语言编写的应用程序，使用malloc申请内存空间，显示窗口随机绘制多个点
********************************************************************************/

#include "apilib.h"

int rand();	// 编译器自带函数 0~32767随机数

void Main() {
	char *buf;
	int win, i, x, y;
	apiInitMalloc();
	buf = apiMalloc(150 * 100);
	win = apiOpenWindow(buf, 150, 100, -1, "stars");
	apiBoxFillWin(win, 6, 26, 143, 93, 0);	// 黑色
	for (i = 0; i < 50; i++) {
		x = (rand() % 137) + 6;
		y = (rand() % 67) + 26;
		apiPoint(win, x, y, 3);	// 黄色
	}
	apiEnd();
}
