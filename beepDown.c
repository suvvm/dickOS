/********************************************************************************
* @File name: beepDown.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-12
* @Description: c语言编写的应用程序 使蜂鸣器发声
*	每隔0.01秒降低发声频率 至20Hz或用户按键停止
********************************************************************************/

#include "apilib.h"

void Main() {
	int i, timer;
	timer = apiAllocTimer();
	apiInitTimer(timer, 128);
	
	for (i = 20000000; i >= 20000; i -= i / 100) {
		// 20KHz ~ 20Hz人耳识别范围 以%1的速度递减
		apiBeep(i);
		apiSetTimer(timer, 1); // 0.01s
		if (apiGetKey(1) != 128) {
			break;
		}
	}
	apiBeep(0);	// 关闭蜂鸣器
	apiEnd();
}
