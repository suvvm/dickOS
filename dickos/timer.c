#ifndef TIMER_C
#define TIMER_C
/********************************************************************************
* @File name: timer.c
* @Author: suvvm
* @Version: 0.0.7
* @Date: 2020-02-11
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
	int i;
	struct TIMER *t;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;	// /定时器中断次数初始化为0
	for (i = 0; i < MAX_TIMER; i++) {
		timerctl.timer[i].status = 0;	// 初始化所有定时器未使用
	}
	t = timerAlloc();	// 先取得一个可用定时器
	t->timeout = 0xffffffff;	//  预留哨兵
	t->status = TIMER_USING;
	t->next = 0;
	timerctl.timerHead = t;	// 只有哨兵，所以哨兵为链表头结点
	timerctl.next = 0xffffffff;	// 下一个超时时间为哨兵超时时间
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
			timerctl.timer[i].flags = 0;
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
void timerInit(struct TIMER *timer, struct QUEUE *queue, int data) {
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
	t = timerctl.timerHead;	// 由于有哨兵，所以一定存在链表头 不会出现只有新的定时器一个和插入链表尾的情况（哨兵总是在最后）
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
		if (timer->timeout <= t->timeout) {	// 找到位置，插入s与t之间
			s->next = timer;
			timer->next = t;
			io_store_eflags(eflags);	// 恢复eflags（恢复中断位开放中断）
			return;
		}
	}
}


/********************************************************
*
* Function name:	timerCancle
* Description: 取消定时器运行
* Parameter:
*	@timer	要取消定时器指针	struct TIMER *
* Return:
*	取消成功返回1不需要取消返回0
*
**********************************************************/
int timerCancle(struct TIMER *timer) {
	int eflags;
	struct TIMER *t;
	eflags = io_load_eflags();
	io_cli();
	if (timer->status == TIMER_USING) {	// 处于运行态
		if (timer == timerctl.timerHead) { // 要取消的定时器处于链表头
			t = timer->next;
			timerctl.timerHead = t;
			timerctl.next = t->timeout;
		} else {
			t = timerctl.timerHead;
			for (;;) {
				if (t->next == timer) {
					break;
				}
				t = t->next;
			}
			t->next = timer->next;
		}
		timer->status = TIMER_ALLOC;
		io_store_eflags(eflags);
		return 1;
	}
	io_store_eflags(eflags);
	return 0;
}

/********************************************************
*
* Function name:	timerCancelAllFlags
* Description: 取消指定缓冲区所有flags不为0的定时器
* Parameter:
*	@queue	要取消定时器的缓冲区	struct QUEUE *
*
**********************************************************/
void timerCancelAllFlags(struct QUEUE *queue) {
	int eflags, i;
	struct TIMER *t;
	eflags = io_load_eflags();
	io_cli();
	for (i = 0; i < MAX_TIMER; i++) {
		t = &timerctl.timer[i];
		if (t->status != 0 && t->flags != 0 && t->queue == queue) {
			timerCancle(t);
			timerFree(t);
		}
	}
	io_store_eflags(eflags);
}
#endif // TIMER_C
