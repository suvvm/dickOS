/********************************************************************************
* @File name: interrupt.c
* @Author: suvvm
* @Version: 1.0.1
* @Date: 2019-10-16
* @Description: 中断操作
********************************************************************************/

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
