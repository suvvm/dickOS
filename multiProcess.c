#ifndef MULTIPOCESS_C
#define MULTIPOCESS_C
/********************************************************************************
* @File name: multiProcess.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-02
* @Description: 定义启动多进程操作相关变量与函数
********************************************************************************/

#include "bootpack.h"
#include "timer.c"

struct TIMER *mpTimer;	// 进程切换定时器
int mpTr;	// 活动进程TR值

/*******************************************************
*
* Function name: multiProcessInit
* Description: 多进程初始化
*
*******************************************************/
void multiProcessInit() {
	mpTimer = timerAlloc();
	// 不使用timerInit 因为超时后不向缓冲区中写入数据
	timerSetTime(mpTimer, 2);	// 0.02秒超时一次
	mpTr = 3 * 8;	// 初始活动进程
}

/*******************************************************
*
* Function name: processSwitch
* Description: 进程切换
*
*******************************************************/
void processSwitch() {
	if (mpTr == 3 * 8) {
		mpTr = 4 * 8;
	} else {
		mpTr = 3 * 8;
	}
	timerSetTime(mpTimer, 2);
	farJmp(0, mpTr);
}

#endif // MULTIPOCESS_C
