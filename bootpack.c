/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 1.0.8
* @Date: 2019-10-24
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
	char mcursor[256], s[40], keyb[32];	// mcursor鼠标信息 s保存要输出的变量信息
	int mx, my, keybufval, i;
	
	initGdtit();	// 初始化GDT IDT
	init_pic();	// 初始化可编程中断控制器
	
	io_sti();	// 解除cpu中断禁止
	
	QueueInit(&keybuf, 32, keyb);
	
	io_out8(PIC0_IMR, 0xf9); // 允许PIC1（从）和键盘(11111001) 
	io_out8(PIC1_IMR, 0xef); //鼠标(11101111)
	
	initKeyboard();
	
	init_palette();	// 初始化16色调色板
	
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;	// 获取启动信息
	mx = (binfo->scrnx - 16) / 2;	// 鼠标x轴位置
	my = (binfo->scrny - 28 - 16) / 2;	// 鼠标y轴位置
	init_GUI(binfo->vram, binfo->scrnx, binfo->scrny);	// 初始化GUI背景
	
	// 打印DICKOS
	putFont8_asc(binfo->vram, binfo->scrnx, 30, 30, COL8_FFFFFF, "SHOWSTRING");
	initMouseCursor8(mcursor, COL8_008484);	// 初始化鼠标信息
	// 根据鼠标信息打印鼠标
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
	sprintf(s, "(%d, %d)", mx, my);	// 将鼠标位置存入s
	// 打印s
	putFont8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
	
	io_out8(PIC0_IMR, 0xf9); // 主PIC IRQ1（键盘）与IRQ2（从PIC）不被屏蔽(11111001)
	io_out8(PIC1_IMR, 0xef); // 从PIC IRQ12（鼠标）不被控制(11101111)
	
	enableMouse();
	
	//处理键盘与鼠标中断与进入hlt
	for(;;){
		io_cli();
		
		if (QueueSize(&keybuf) == 0) {
			io_stihlt();
		} else {
			keybufval = QueuePop(&keybuf);
			io_sti();
			sprintf(s, "%02X", keybufval);
			boxFill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
			putFont8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
		}
	}
}
