/********************************************************************************
* @File name: timer.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-01-30
* @Description: 定义定时器相关函数
********************************************************************************/
#include "bootpack.h"

struct TIMERCTL timerctl;

/********************************************************
*
* Function name:	initPit
* Description:	初始化可编程间隔型定时器
*
**********************************************************/
void initPit() {
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;
}

/********************************************************
*
* Function name:	interruptHandler20
* Description: IRQ0中断处理程序
* Parameter:
* 	@esp	接收指针的值
*
**********************************************************/
void interruptHandler20(int *esp) {
	io_out8(PIC0_OCW2, 0x60);	// 将IRQ-0信号接收完毕的消息通知给PIC
	timerctl.count++;	// 计时器控制块中的计数
}
