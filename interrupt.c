/********************************************************************************
* @File name: interrupt.c
* @Author: suvvm
* @Version: 1.0.5
* @Date: 2019-10-22
* @Description: 中断操作
********************************************************************************/

#include "bootpack.h"

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
	unsigned char data;
	io_out8(PIC0_OCW2, 0x61);	//通知PIC IRQ-01 已经受理完毕 0x60 + IRQ编号
	data = io_in8(PORT_KEYDAT);
	if(!keybuf.next < 32){
		keybuf.data[keybuf.next] = data;
		keybuf.next++;
	}
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
* Description: 接收来自IRQ12 鼠标的中断并显示提示信息
* Parameter:
* 	@esp	接收指针的值
*
*******************************************************/
void interruptHandler2c(int *esp){
	// 获取启动信息
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	// 显示信息背景
	boxFill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32 * 8 - 1, 15);
	// 显示提示信息
	putFont8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "INT 2C (IRQ-12) : PS/2 mouse");
	
	for (;;) {
		io_hlt();
	}
}
