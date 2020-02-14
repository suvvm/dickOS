/********************************************************************************
* @File name: noodle.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-11
* @Description: c语言编写的应用程序 定时器计时显示时分秒
********************************************************************************/

#include <stdio.h>
#include "apilib.h"

void Main() {
	char *buf, s[12];
	int win, timer, sec = 0, min = 0, hou = 0;	// 窗口句柄 定时器句柄 秒 分 时
	apiInitMalloc();
	buf = apiMalloc(150 * 50);
	win = apiOpenWindow(buf, 150, 50, -1, "noodle");
	timer = apiAllocTimer();
	apiInitTimer(timer, 128);	// 超时数据注意不要与键盘数据冲突
	for (;;) {
		sprintf(s, "%5d:%02d:%02d", hou, min, sec);
		apiBoxFillWin(win, 28, 27, 115, 41, 7);	// 白色
		apiPutStrWin(win, 28, 27, 0, 11, s);	// 黑色
		apiSetTimer(timer, 100);	// 1s
		if (apiGetKey(1) != 128) {	// 任意按键退出
			break;
		}
		sec++;
		if (sec == 60) {
			sec = 0;
			min++;
			if (min == 60) {
				min = 0;
				hou++;
			}
		}
	}
	apiEnd();
}
