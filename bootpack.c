/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 1.0.10
* @Date: 2019-10-28
* @Description: 包含启动后要使用的功能函数
********************************************************************************/
#include "bootpack.h"
#include "desctab.c"
#include "graphic.c"
#include "queue.h"
#include "mouse.c"

/*******************************************************
*
* Function name:Main
* Description: 主函数
*
**********************************************************/
void Main(){
	struct BOOTINFO *binfo;
	char mcursor[256], s[40], keyb[32], mouseb[128];	// mcursor鼠标信息 s保存要输出的变量信息
	int mx, my, bufval; //鼠标x轴位置 鼠标y轴位置 要显示的缓冲区信息
	unsigned char mouseDbuf[3], mousePhase;
	
	initGdtit();	// 初始化GDT IDT
	init_pic();	// 初始化可编程中断控制器
	io_sti();	// 解除cpu中断禁止
	
	QueueInit(&keybuf, 32, keyb);	//初始化键盘缓冲区队列
	QueueInit(&mousebuf, 128, mouseb);	// 初始化鼠标缓冲区队列
	
	io_out8(PIC0_IMR, 0xf9); // 允许PIC1（从）和键盘(11111001) 
	io_out8(PIC1_IMR, 0xef); //鼠标(11101111)
	
	initKeyboard();
	
	init_palette();	// 初始化16色调色板
	
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;	// 获取启动信息
	mx = (binfo->scrnx - 16) / 2;	// 鼠标x轴位置
	my = (binfo->scrny - 28 - 16) / 2;	// 鼠标y轴位置
	init_GUI(binfo->vram, binfo->scrnx, binfo->scrny);	// 初始化GUI背景
	
	// 打印DICKOS
	putFont8_asc(binfo->vram, binfo->scrnx, 30, 35, COL8_FFFFFF, "SHOWSTRING");
	initMouseCursor8(mcursor, COL8_008484);	// 初始化鼠标信息
	// 根据鼠标信息打印鼠标
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
	sprintf(s, "(%d, %d)", mx, my);	// 将鼠标位置存入s
	// 打印s
	putFont8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
	
	io_out8(PIC0_IMR, 0xf9); // 主PIC IRQ1（键盘）与IRQ2（从PIC）不被屏蔽(11111001)
	io_out8(PIC1_IMR, 0xef); // 从PIC IRQ12（鼠标）不被控制(11101111)
	
	enableMouse();
	mousePhase = 0;	//此时为等待鼠标传回激活回复0xfa
	//处理键盘与鼠标中断与进入hlt
	for(;;){
		io_cli();
		if(QueueSize(&keybuf) + QueueSize(&mousebuf) == 0) {	// 只有在两个缓冲区都没有数据时才能开启中断并进入hlt模式
			io_stihlt();
		} else {
			if (QueueSize(&keybuf) != 0) {
				bufval = QueuePop(&keybuf);
				io_sti();
				sprintf(s, "%02X", bufval);
				boxFill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
				putFont8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
			} else {
				bufval = QueuePop(&mousebuf);
				io_sti();
				if (mousePhase == 0) {	//鼠标传回激活回复0xfa
					if (bufval == 0xfa)
						mousePhase = 1;
				} else if (mousePhase == 1) {	// 鼠标传回第一字节数据
					mouseDbuf[0] = bufval;
					mousePhase = 2;
				} else if (mousePhase == 2) {	// 鼠标传回第二字节数据
					mouseDbuf[1] = bufval;
					mousePhase = 3;
				} else if (mousePhase == 3) {	// 鼠标传回第三字节数据
					mouseDbuf[2] = bufval;
					mousePhase = 1;	// 准备接收下一次鼠标数据
					// 三个字节接收完毕打印其信息
					sprintf(s, "%02X %02X %02X", mouseDbuf[0], mouseDbuf[1], mouseDbuf[2]);
					boxFill8(binfo->vram, binfo->scrnx, COL8_008484, 32, 16, 32 + 8 * 8 - 1, 31);
					putFont8_asc(binfo->vram, binfo->scrnx, 32, 16, COL8_FFFFFF, s);
				} 
			}
		}
		/*
		if (QueueSize(&keybuf) == 0) {
			io_stihlt();
		} else {
			keybufval = QueuePop(&keybuf);
			io_sti();
			sprintf(s, "%02X", keybufval);
			boxFill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
			putFont8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
		}*/
	}
}
