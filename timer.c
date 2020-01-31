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
	timerctl.nowUsing = 0;
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
	int eflags, i, j;
	timer->timeout = timeout + timerctl.count;
	timer->status = TIMER_USING;	// 定时器正在运行
	eflags = io_load_eflags();	// 保存eflags状态
	io_cli();	// 关中断
	for (i = 0; i < timerctl.nowUsing; i++) {	// 找到当前定时器在timerAcs中应插入的位置
		if (timerctl.timerAcs[i]->timeout >= timer->timeout)
			break;	
	}
	for (j = timerctl.nowUsing; j > i; j--) {	// i位置之后的元素后移
		timerctl.timerAcs[j] = timerctl.timerAcs[j - 1];
	}
	timerctl.nowUsing++;	// 当运行寄存器数量加一
	
	timerctl.timerAcs[i] = timer;	// 将当前定时器插入指定位置
	timerctl.next = timerctl.timerAcs[0]->timeout;	// 更新下一次超时的时限
	io_store_eflags(eflags);	// 恢复eflags（恢复中断位开放中断）
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
	for (i = 0; i < timerctl.nowUsing; i++) {	// 处理所有超时定时器并记录个数
		if (timerctl.timerAcs[i]->timeout > timerctl.count) {	// i定时器未超时
			break;
		}
		// 超时
		timerctl.timerAcs[i]->status = TIMER_ALLOC;	// 定时器设为已分配可使用态
		QueuePush(timerctl.timerAcs[i]->queue, timerctl.timerAcs[i]->data);	// 对应超时信息入队对应缓冲区队列
	}
	// i现在的值为超时定时器数量
	timerctl.nowUsing -= i;	// 正在运行定时器数量减去已经超时的定时器
	int j;
	for (j = 0; j < timerctl.nowUsing; j++) {	// 将剩余正在运行的定时器前移
		timerctl.timerAcs[j] = timerctl.timerAcs[i + j];
	}
	if (timerctl.nowUsing > 0) {	// 若还有正在运行的定时器
		timerctl.next = timerctl.timerAcs[0]->timeout;	// 更新下一个超时时限
	} else {	// 没有正在运行的定时器
		timerctl.next = 0xffffffff;
	}
}
