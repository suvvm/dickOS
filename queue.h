#ifndef QUEUE_H
#define QUEUE_H
/********************************************************************************
* @File name: queue.c
* @Author: suvvm
* @Version: 1.0.1
* @Date: 2019-10-23
* @Description: 队列操作
********************************************************************************/

#include "bootpack.h"

/*******************************************************
*
* Function name: QueueInit
* Description: 初始化队列
* Parameter:
* 	@q	要初始化的队列
* 	@size	队列的上限
* 	@buf	队列数据空间
*
**********************************************************/
void QueueInit(struct QUEUE *q, int size, unsigned char *buf){
	q->size = size;	// 初始队列长度
	q->buf = buf;	// 初始化数据空间
	q->free = size;	// 队列初始化为空
	q->flags = 0;	// 没有溢出记录
	q->back = q->front = 0;	// 初始化队首队尾
}

/*******************************************************
*
* Function name: QueuePush
* Description: 入队函数
* Parameter:
* 	@q	要执行入队操作的队列
* 	@data	要入队的数据
* return: 如果数据没有溢出返回0 否则返回-1
*
**********************************************************/
int QueuePush(struct QUEUE *q, unsigned char data){
	if (q->free == 0) {
		q->flags = FLAGS_OVERRUN;
		return -1;
	}
	q->buf[q->back] = data;
	q->back++;
	q->back %= q->size;
	q->free--;
	return 0;
}

/*******************************************************
*
* Function name: QueuePop
* Description: 出队函数
* Parameter:
* 	@q	要执行出队操作的队列
* return: 如果数据不为空返回队首数据 否则返回-1
*
**********************************************************/
int QueuePop(struct QUEUE *q){
	int data;
	if (q->free == q->size) {	// 判断队列是否为空
		return -1;
	}
	data = q->buf[q->front];
	q->front++;
	q->front %= q->size;
	q->free++;
	return data;
}

/*******************************************************
*
* Function name: QueueSize
* Description: 获取当前队列长度
* Parameter:
* 	@q	要获得长度的队列
* return: 返回队列长度
*
**********************************************************/
int QueueSize(struct QUEUE *q){
	return q->size - q->free;
}

#endif // QUEUE_H