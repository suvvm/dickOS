/********************************************************************************
* @File name: timer.c
* @Author: suvvm
* @Version: 0.0.5
* @Date: 2020-01-31
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
	timerctl.count = 0;	// /定时器中断次数初始化为0
	timerctl.next = 0xffffffff;	// 初始没有正在运行的定时器
	int i;
	for (i = 0; i < MAX_TIMER; i++) {
		timerctl.timer[i].status = 0;	// 初始化所有定时器未使用
	}
}

/********************************************************
*
* Function name:	timerAlloc
* Description:	取得新的未使用的定时器
* Return:
*	成功返回指向对应定时器的指针，失败返回0
*
**********************************************************/
struct TIMER *timerAlloc() {
	int i;
	for (i = 0; i < MAX_TIMER; i++) {
		if (timerctl.timer[i].status == 0) {
			timerctl.timer[i].status = TIMER_ALLOC;
			return &timerctl.timer[i];
		}
	}
	return 0;
}

/********************************************************
*
* Function name:	timerFree
* Description:	释放目标定时器
* Parameter:
*	@timer	欲释放定时器指针	struct TIMER *
*
**********************************************************/
void timerFree(struct TIMER *timer) {
	timer->status = 0;	// 标记为未使用
}

/********************************************************
*
* Function name:	timerInit
* Description:	初始化定时器
* Parameter:
*	@timer	欲初始化定时器指针	struct TIMER *
*	@queue	定时器缓冲区队列	struct QUEUE *
*	@data	定时器超时信息		unsigned char
*
**********************************************************/
void timerInit(struct TIMER *timer, struct QUEUE *queue, unsigned char data) {
	timer->queue = queue;
	timer->data = data;
}

/********************************************************
*
* Function name:	timerSetTime
* Description: 设置定时器超时时限
* Parameter:
*	@timer		定时器指针			struct TIMER *
* 	@timeout	定时器超时时限		unsigned int
*
**********************************************************/
void timerSetTime(struct TIMER *timer, unsigned int timeout) {
	timer->timeout = timeout + timerctl.count;
	timer->status = TIMER_USING;	// 定时器正在运行
	if (timerctl.next > timer->timeout) {	// 若新的超时时间早于当前下一次超时时间
		timerctl.next = timer->timeout;	// 更新下一次超时时间
	}
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
	int i;
	io_out8(PIC0_OCW2, 0x60);	// 将IRQ-0信号接收完毕的消息通知给PIC
	timerctl.count++;	// 计时器控制块中的计数
	if (timerctl.next > timerctl.count) {	// 还不到下一个超时时刻
		return;
	}
	for (i = 0; i < MAX_TIMER; i++) {	
		if (timerctl.timer[i].status == TIMER_USING) {	// 如果定时器正在运行
			if (timerctl.timer[i].timeout <= timerctl.count) {	// 若超时就将超时数据写入缓冲队列中(timeout现在表示指定超时时限，不在发生变动以减少中断处理时间)
				timerctl.timer[i].status = TIMER_ALLOC;	// 定时器设为已分配可使用态
				QueuePush(timerctl.timer[i].queue, timerctl.timer[i].data);	// 对应超时信息入队对应缓冲区队列
			} else {	// 当前定时器还未超时
				if (timerctl.next > timerctl.timer[i].timeout) {	// 判断是否需要更新下一个超时时刻的值
					timerctl.next = timerctl.timer[i].timeout;
				}
			}
		}	
	}
}
