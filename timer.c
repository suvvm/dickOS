/********************************************************************************
* @File name: timer.c
* @Author: suvvm
* @Version: 0.0.2
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
	timerctl.count = 0;	// 计时器中断次数初始化为0
	timerctl.timeout = 0;	// 超时时限初始化为0
	
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
	if (timerctl.timeout > 0) {	// 如果已经设置了超时时限
		timerctl.timeout--;
		if (timerctl.timeout == 0) {	// 若超时就将超时数据写入缓冲队列中
			QueuePush(timerctl.queue, timerctl.data);
		}
	}
}

/********************************************************
*
* Function name:	settimer
* Description: 设置定时器信息
* Parameter:
* 	@timeout	定时器超时时限		unsigned int
*	@queue		定时器缓冲区队列	struct QUEUE *
*	@data		定时器超时信息		unsigned char
*
**********************************************************/
void setTimer(unsigned int timeout, struct QUEUE *queue, unsigned char data) {
	int eflags;
	eflags = io_load_eflags();	// 读取eflags寄存器中的对应硬件信息
	io_cli();	// 关中断 清理IF位
	// 设置定时器信息
	timerctl.timeout = timeout;
	timerctl.queue = queue;
	timerctl.data = data;
	io_store_eflags(eflags);	// 恢复eflags（恢复IF位）
}
