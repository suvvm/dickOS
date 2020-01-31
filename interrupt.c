#ifndef INTERRUPT_C
#define INTERRUPT_C
/********************************************************************************
* @File name: interrupt.c
* @Author: suvvm
* @Version: 0.1.0
* @Date: 2020-01-31
* @Description: 中断操作
********************************************************************************/

#include "bootpack.h"
#include "queue.h"
#include "mouse.c"
#include "keyboard.c"
#include "timer.c"

/*******************************************************
*
* Function name:init_pic
* Description: 初始化可编程中断控制器（PIC相关信息详见note.txt）
*
*******************************************************/
void init_pic(){
	io_out8(PIC0_IMR, 0xff);	/*禁止所有中断 PIC为意为可编程中断控制器 IMR为中断屏蔽寄存器 详见note.txt*/
	io_out8(PIC1_IMR, 0xff);	/*禁止所有中断*/
	
	io_out8(PIC0_ICW1, 0x11);	/*边沿触发模式 edge trrigger mode*/
	io_out8(PIC0_ICW2, 0x20);	/*IRQ0-7（主PIC）由INT20-27接收*/
	io_out8(PIC0_ICW3, 1 << 2);	/*PCI1由IRQ2连接*/
	io_out8(PIC0_ICW4, 0x01);	/*无缓冲区模式*/
	
	io_out8(PIC1_ICW1, 0x11);	/*边沿触发模式 edge trrigger mode*/
	io_out8(PIC1_ICW2, 0x28);	/*IRQ8-15（从PIC）由INT20-27接收*/
	io_out8(PIC1_ICW2, 2);	/*PCI1由IRQ2连接*/
	io_out8(PIC1_ICW2, 0x01);	/*无缓冲区模式*/
	
	io_out8(PIC0_IMR, 0xfb);	/* 11111011 PIC0_ICW1以外全部禁止*/
	io_out8(PIC1_IMR, 0xff);	/* 11111111 禁止所有中断*/
}

/*******************************************************
*
* Function name:interruptHandler21
* Description: 接收来自IRQ2 PS/2键盘的中断并存入缓冲区
* Parameter:
* 	@esp	接收指针的值
*
*******************************************************/
void interruptHandler21(int *esp){
	int data;
	io_out8(PIC0_OCW2, 0x61);	//通知PIC0 IRQ-01 已经受理完毕 0x60 + IRQ编号
	data = io_in8(PORT_KEYDAT);
	QueuePush(keybuf, data + keyData0);
	/* 直接处理方法
	sprintf(s, "%02X", data);
	boxFill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
	putFont8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
	*/
}

/*******************************************************
*
* Function name:interruptHandler27
* Description: 接收来自IRQ7 PIC初始化中断 
*	针对来自PIC0的不完全中断，在某些机型中等由于芯片组的情况PIC的初始化时会发送IRQ7中断，
*	如果不对该中断处理程序执行STI（设置中断标指位），操作系统会启动失败
* Parameter:
* 	@esp	接收指针的值
*
*******************************************************/
void interruptHandler27(int *esp){
	io_out8(PIC0_OCW2, 0x67);
	return;
}

/*******************************************************
*
* Function name:interruptHandler2c
* Description: 接收来自IRQ12 鼠标的中断并存入缓冲区
* Parameter:
* 	@esp	接收指针的值
*
*******************************************************/
void interruptHandler2c(int *esp){
	int data;
	io_out8(PIC1_OCW2, 0x64);	// 通知PIC1 IRQ-12 已经受理完毕 0x60 + IRQ编号
	io_out8(PIC0_OCW2, 0x62);	// 通知PIC0 IRQ-02 已经受理完毕 0x60 + IRQ编号
	data = io_in8(PORT_KEYDAT);
	QueuePush(mousebuf, data + mouseData0);
}


/********************************************************
*
* Function name:	interruptHandler20
* Description: IRQ0定时器中断处理程序
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

#endif // INTERRUPT_C
