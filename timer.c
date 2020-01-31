#ifndef TIMER_C
#define TIMER_C
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
	int eflags;
	struct TIMER *t, *s;
	timer->timeout = timeout + timerctl.count;
	timer->status = TIMER_USING;	// 定时器正在运行
	eflags = io_load_eflags();	// 保存eflags状态
	io_cli();	// 关中断
	timerctl.nowUsing++;	// 当前运行寄存器数量加一
	if (timerctl.nowUsing == 1) {	// 处于运行态的定时器只有新加的这一个
		timerctl.timerHead = timer;
		timer->next = 0;	// 链表只有头结点
		timerctl.next = timer->timeout;	// 更新下一次超时的时限
		io_store_eflags(eflags);	// 恢复eflags（恢复中断位开放中断）
		return;
	}
	t = timerctl.timerHead;	// t为先前运行寄存器链表头节点
	if (timer->timeout <= t->timeout) {	// 新的定时器超时时间比头结点短 头插
		timerctl.timerHead = timer;
		timer->next = t;
		timerctl.next = timer->timeout;	// 更新下一次超时的时限
		io_store_eflags(eflags);	// 恢复eflags（恢复中断位开放中断）
		return;
	}
	for (;;) {	// 插入链表中间对应位置
		s = t;
		t = t->next;
		if (t == 0) {	// 链表尾
			break;
		}
		
		if (timer->timeout <= t->timeout) {	// 找到位置，插入s与t之间
			s->next = timer;
			timer->next = t;
			io_store_eflags(eflags);	// 恢复eflags（恢复中断位开放中断）
			return;
		}
	}
	// 插入链表尾
	// s当前指向链表尾
	s->next = timer;
	timer->next = 0;	// timer为新的链表尾
	io_store_eflags(eflags);	// 恢复eflags（恢复中断位开放中断）
}

#endif // TIMER_C
