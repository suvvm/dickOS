/********************************************************************************
* @File name: winHelo.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-09
* @Description: c语言编写的应用程序用于显示窗口并打印hello
********************************************************************************/

#include "apilib.h"

void Main () {
	int win;
	char buf[150 * 50];
	win = apiOpenWindow(buf, 150, 50, -1, "hello");
	for (;;) {
		if (apiGetKey(1) == 0x0a) {
			break;
		}
	}
	apiEnd();
}
