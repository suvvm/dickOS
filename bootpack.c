/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 0.3.1
* @Date: 2020-02-02
* @Description: 包含启动后要使用的功能函数
********************************************************************************/
#include "bootpack.h"
#include "desctab.c"
#include "graphic.c"
#include "queue.h"
#include "mouse.c"
#include "keyboard.c"
#include "memory.c"
#include "sheet.c"
#include "timer.c"

/********************************************************
*
* Function name: taskBmain
* Description: 任务B的主函数
*
**********************************************************/
void taskBmain() {
	for(;;) {
		io_hlt();
	}
}

/*******************************************************
*
* Function name: makeTextBox
* Description: 创建文本输入框
* Parameter:
*	@sheet	图称指针	struct SHEET *
*	@startX	x轴起始位置	int
*	@startY	y轴起始位置	int
*	@width	宽度		int
*	@height	高度		int
*	@c		文本框颜色	int
*
**********************************************************/
void makeTextBox(struct SHEET *sheet, int startX, int startY, int width, int height, int c) {
	int endX = startX + width, endY = startY + height;
	boxFill8(sheet->buf, sheet->width, COL8_848484, startX - 2, startY - 3, endX + 1, startY - 3);	// 暗灰色 (startX-2,startY-3)~(endX+1,startY-3) 一条上部边框横线
	boxFill8(sheet->buf, sheet->width, COL8_848484, startX - 3, startY - 3, startX - 3, endY + 1);	// 暗灰色 (startX-3,startY-3)~(startX-3,endY+1) 一条左侧边框竖线
	boxFill8(sheet->buf, sheet->width, COL8_FFFFFF, startX - 3, endY + 2, endX + 1, endY + 2);		// 白色 (startX-3,endY+2)~(endX+1,endY+2) 一条底部边框横线
	boxFill8(sheet->buf, sheet->width, COL8_FFFFFF, endX + 2, startY - 3, endX + 2, endY + 2);		// 白色 (endX+2,startY-2)~(endX+2,endY+2) 一条右侧边框竖线
	boxFill8(sheet->buf, sheet->width, COL8_000000, startX - 1, startY - 2, endX, startY - 2);		// 黑色	(startX-1,startY-2)~(endX,startY-2) 一条顶部边框横线
	boxFill8(sheet->buf, sheet->width, COL8_000000, startX - 2, startY - 2, startX - 2, endY);		// 黑色 (startX-2,startY-2)~(startX-2,endY) 一条左侧边框竖线
	boxFill8(sheet->buf, sheet->width, COL8_C6C6C6, startX - 2, endY + 1, endX, endY + 1);			// 亮灰色 (startX-2,endY+1,endX,endY+1) 一条底部边框横线
	boxFill8(sheet->buf, sheet->width, COL8_C6C6C6, endX + 1, startY - 2, endX + 1, endY + 1);		// 亮灰色 (endX+1,startY-2)~(endX+1,endY+1) 一条右侧边框竖线
	
	boxFill8(sheet->buf, sheet->width, c, startX - 1, startY - 1, endX, endY);	// 文本输入区
}

/*******************************************************
*
* Function name: makeWindow
* Description: 创建窗口
* Parameter:
*	@buf	窗口图像缓冲区	unsigned char *
*	@width	窗口宽度		int
*	@height	窗口高度		int
*	@title	窗口标题		char *
*
**********************************************************/
void makeWindow(unsigned char *buf, int width, int height, char *title) {
	static char closeBtn[14][16] = {	// 关闭按钮
		"OOOOOOOOOOOOOOO@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQQQ@@QQQQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"O$$$$$$$$$$$$$$@",
		"@@@@@@@@@@@@@@@@"
	};
	int x, y;
	char c;
	// 绘制窗口（坐标都为相对坐标，起始(0,0)）
	boxFill8(buf, width, COL8_C6C6C6, 0, 0, width - 1, 0);	// 亮灰色 (0,0)~(width-1,0)顶部横线
	boxFill8(buf, width, COL8_FFFFFF, 1, 1, width - 2, 1);	// 白色 (1,1)~(width-2,1)顶部横线
	boxFill8(buf, width, COL8_C6C6C6, 0, 0, 0, height - 1);	// 亮灰色 (0,0)~(0,height-1)左侧竖线
	boxFill8(buf, width, COL8_FFFFFF, 1, 1, 1, height - 2);	// 白色 (1,1)~(1,height-2)左侧竖线
	boxFill8(buf, width, COL8_848484, width - 2, 1, width - 2, height - 2);	//暗灰色 (width - 2,1)~(width - 2, height - 2)一条右侧竖线
	boxFill8(buf, width, COL8_000000, width - 1, 0, width - 1, height - 1);	// 黑色 (width - 1,0)~(width - 1, height - 1)一条右侧竖线
	boxFill8(buf, width, COL8_C6C6C6, 2, 2, width - 3, height - 3);	// 亮灰色 (2,2)~(width - 3,height - 3) 中心矩形
	boxFill8(buf, width, COL8_000084, 3, 3, width - 4, 20);	// 暗青色(3,3)~(width - 4, 20)顶部矩形
	boxFill8(buf, width, COL8_848484, 1, height - 2, width - 2, height - 2);	// 暗灰色(1,height-2)~(width-2,height-2)底部横线
	boxFill8(buf, width, COL8_000000, 0, height - 1, width - 1, height - 1);	// 黑色(0,height-1)~(width-1,height-1)底部横线
	putFont8_asc(buf, width, 24, 4, COL8_FFFFFF, title);
	for (y = 0; y < 14; y++) {
		for (x = 0; x < 16; x++) {
			c = closeBtn[y][x];
			if (c == '@') {
				c = COL8_000000;
			} else if (c == '$') {
				c = COL8_848484;
			} else if (c == 'Q') {
				c = COL8_C6C6C6;
			} else {
				c = COL8_FFFFFF;
			}
			buf[(5 + y) * width + (width - 21 + x)] = c; 
		}
	}
}

/*******************************************************
*
* Function name:Main
* Description: 主函数
*
**********************************************************/
void Main(){
	struct BOOTINFO *binfo;
	char s[40];
	int	buf[128];	// s保存要输出的变量信息 buf为总缓冲区
	int mx, my, bufval, cursorX, cursorC, taskBesp; //鼠标x轴位置 鼠标y轴位置 光标x轴位置 光标颜色
	struct MouseDec mdec;	// 保存鼠标信息
	unsigned int memtotal;
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;	// 内存段表指针
	struct SHTCTL *shtctl;	// 图层控制块指针
	struct SHEET *sheetBack, *sheetMouse, *sheetWin;	// 背景图层 鼠标图层 窗口图层
	unsigned char *bufBack, bufMouse[256], *bufWin;	// 背景图像缓冲区 鼠标图像缓冲区 窗口图像缓冲区
	struct QUEUE queue;	// 总缓冲区
	struct TIMER *timer1, *timer2, *timer3;	// 三个定时器指针
	struct TSS32 tssA, tssB;	// 任务A控制段 任务B控制段
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;	// 段描述符存放GDT内容
	
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;	// 获取启动信息
	
	initGdtit();	// 初始化GDT IDT
	init_pic();	// 初始化可编程中断控制器
	io_sti();	// 解除cpu中断禁止
		
	QueueInit(&queue, 128, buf);	// 初始化缓冲区队列
	
	initPit();	// 初始化定时器
	initKeyboard(&queue, 256);	// 初始化键盘控制电路
	enableMouse(&queue, 512, &mdec);	// 激活鼠标
	
	io_out8(PIC0_IMR, 0xf8); // 主PIC IRQ0(定时器) IRQ1（键盘）与IRQ2（从PIC）不被屏蔽(11111000)
	io_out8(PIC1_IMR, 0xef); // 从PIC IRQ12（鼠标）不被控制(11101111)
	
	timer1 = timerAlloc();	// 获取一个可使用的定时器
	timerInit(timer1, &queue, 10);	// 初始化定时器1
	timerSetTime(timer1, 1000);

	timer2 = timerAlloc();	// 获取一个可使用的定时器
	timerInit(timer2, &queue, 3);	// 初始化定时器1
	timerSetTime(timer2, 300);

	timer3 = timerAlloc();	// 获取一个可使用的定时器
	timerInit(timer3, &queue, 1);	// 初始化定时器1
	timerSetTime(timer3, 50);
	
	memtotal = memtest(0x00400000, 0xbfffffff);	// 获取内存总和
	memsegInit(memsegtable);
	memsegFree(memsegtable, 0x00001000, 0x0009e000);
	memsegFree(memsegtable, 0x00400000, memtotal - 0x00400000);
	
	init_palette();	// 初始化16色调色板
	
	shtctl = shtctlInit(memsegtable, binfo->vram, binfo->scrnx, binfo->scrny);
	sheetBack = sheetAlloc(shtctl);
	sheetMouse = sheetAlloc(shtctl);
	sheetWin = sheetAlloc(shtctl);
	bufBack = (unsigned char *) memsegAlloc4K(memsegtable, binfo->scrnx * binfo->scrny);	// 以4KB为单位为背景分配内存
	bufWin = (unsigned char *) memsegAlloc4K(memsegtable, 160 * 52);
	
	sheetSetbuf(sheetBack, bufBack, binfo->scrnx, binfo->scrny, -1);	// 设置背景图层缓冲区，背景不需要透明色设为-1
	sheetSetbuf(sheetMouse, bufMouse, 16, 16, COL8_008484);	// 设置鼠标图层缓冲区与透明色
	sheetSetbuf(sheetWin, bufWin, 160, 52, -1);	// 设置窗口图层缓冲区 无透明色
	
	init_GUI(bufBack, binfo->scrnx, binfo->scrny);	// 初始化GUI至bufBack
	initMouseCursor8(bufMouse, COL8_008484);	// 初始化鼠标至bufMouse
	
	makeWindow(bufWin, 160, 52, "window");	// 显示窗口
	makeTextBox(sheetWin, 8, 28, 144, 16, COL8_FFFFFF);	// 显示文本输入区
	cursorX = 8;	// 设置光标起始位置
	cursorC = COL8_FFFFFF;	// 设置光标颜色
	
	sheetSlide(sheetBack, 0, 0);	// 背景图层起始坐标(0,0)
	// 初始鼠标坐标为屏幕正中
	mx = (binfo->scrnx - 16) / 2;	// 鼠标x轴位置
	my = (binfo->scrny - 28 - 16) / 2;	// 鼠标y轴位置
	sheetSlide(sheetMouse, mx, my);	// 将鼠标图层滑动至对应位置
	sheetSlide(sheetWin, 80, 80);	// 将窗口图层滑动至目标位置
	sheetUpdown(sheetBack, 0);	// 将背景图层置于索引0
	sheetUpdown(sheetWin, 1);	// 将窗口图层置于背景之上鼠标之下
	sheetUpdown(sheetMouse, 2);	// 将鼠标图层置于背景层上方索引1
	
	putFont8AscSheet(sheetBack, 30, 32, COL8_FFFFFF, COL8_008484,  "DickOS", 6);	// 将DickOS写入背景层
	sprintf(s, "(%3d, %3d)", mx, my);	// 将鼠标位置存入s
	putFont8AscSheet(sheetBack, 0, 0, COL8_FFFFFF, COL8_008484,  s, 10);	// 将s写入背景层
	sprintf(s, "memory %dMB free : %dKB", memtest(0x00400000, 0xbfffffff) / (1024 * 1024), memsegTotal(memsegtable) / 1024);	// 将内存信息存入s
	putFont8AscSheet(sheetBack, 0, 48, COL8_FFFFFF, COL8_008484,  s, 26);	// 将s写入背景层
	
	// 为两个任务的ldtr与iomap赋值
	tssA.ldtr = 0;
	tssA.iomap = 0x40000000;
	tssB.ldtr = 0;
	tssB.iomap = 0x40000000;
	
	// 在全局描述符表中定义两个任务状态段
	setSegmdesc(gdt + 3, 103, (int) &tssA, AR_TSS32);	// 将TSSA添加至GDT3号 段长上限103字节
	setSegmdesc(gdt + 4, 103, (int) &tssB, AR_TSS32);	// 将TSSB添加至GDT4号 段长上限103字节
	
	loadTr(3 * 8);	// 为TR寄存器赋值 让cpu记住当前运行的任务是任务A
	taskBesp = memsegAlloc4K(memsegtable, 64 * 1024) + 64 * 1024;	// 为任务B栈分配64KB内存并计算栈底地址给B的ESP
	tssB.eip = (int) &taskBmain;	// B的下一条指令执行taskBmain
	tssB.eflags = 0x00000202;	// 中断标准IF位为1
	tssB.eax = 0;
	tssB.ecx = 0;
	tssB.edx = 0;
	tssB.ebx = 0;
	tssB.esp = taskBesp;
	tssB.ebp = 0;
	tssB.esi = 0;
	tssB.edi = 0;
	tssB.es = 1 * 8;
	tssB.cs = 2 * 8;
	tssB.ss = 1 * 8;
	tssB.ds = 1 * 8;
	tssB.fs = 1 * 8;
	tssB.gs = 1 * 8;
	
	//处理中断与进入hlt
	for(;;){
		// count++;
		// sprintf(s, "%010d", timerctl.count);	
		// putFont8AscSheet(sheetWin, 40, 28, COL8_000000, COL8_C6C6C6,  s, 10);	// 显示定时器计时

		io_cli();	// 关中断
		if(QueueSize(&queue) == 0) {	// 只有缓冲区没有数据时才能开启中断并进入hlt模式
			io_stihlt();	// 开中断进入htl
		} else {
			bufval = QueuePop(&queue);	// 取出缓冲区队列队首数据
			io_sti();	// 开中断
			if (256 <= bufval && bufval <= 511) {	// 键盘中断数据
				sprintf(s, "%02X", bufval - 256);
				putFont8AscSheet(sheetBack, 0, 16, COL8_FFFFFF, COL8_008484, s, 2);	// 将键盘中断信息打印至背景层
				if (bufval < 256 + 0x80) {	// 按下键盘
					if (keyboardTable[bufval - 256] != 0 && cursorX < 144) {	// 键盘对应值有字符且光标没有到输入框末尾
						s[0] = keyboardTable[bufval - 256];
						s[1] = 0;
						putFont8AscSheet(sheetWin, cursorX, 28, COL8_000000, COL8_FFFFFF, s, 1);	// 将字符打印至窗口层
						cursorX += 8;	// 光标后移一个字符
					}
				}
				if (bufval == 256 + 0x0e && cursorX > 8) {	// 按下退格键且光标不在输入框起始位置
					putFont8AscSheet(sheetWin, cursorX, 28, COL8_000000, COL8_FFFFFF, " ", 1);	// 用空格消去当前光标
					cursorX -= 8;	// 光标前移一个字符
				}
				// 重新显示光标
				boxFill8(sheetWin->buf, sheetWin->width, cursorC, cursorX, 28, cursorX + 7, 43);
				sheetRefresh(sheetWin, cursorX, 28, cursorX + 8, 44);
			} else if (512 <= bufval && bufval <= 767) {	// 鼠标中断数据
				if (mouseDecode(&mdec, bufval - 512) != 0) {	// 完成一波三个字节数据的接收或者出现未知差错
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					if ((mdec.btn & 0x01) != 0) {	// 按下左键
						s[1] = 'L';
						sheetSlide(sheetWin, mx - 80, my - 8);
					}
					if ((mdec.btn & 0x02) != 0) {	// 按下右键
						s[3] = 'R';
					}
					if ((mdec.btn & 0x04) != 0) {	// 中间滚轮
						s[2] = 'C';
					}
					putFont8AscSheet(sheetBack, 32, 16, COL8_FFFFFF, COL8_008484, s, 15);	// 在背景层打印鼠标按键信息
					
					// 鼠标坐标加上偏移量
					mx += mdec.x;
					my += mdec.y;
					
					// 超边界处理
					if (mx < 0)
						mx = 0;
					if (my < 0)
						my = 0;
					if (mx > binfo->scrnx - 1)
						mx = binfo->scrnx - 1;
					if (my > binfo->scrny - 1)
						my = binfo->scrny - 1;
					
					sprintf(s, "(%3d, %3d)", mx, my);
					putFont8AscSheet(sheetBack, 0, 0, COL8_FFFFFF, COL8_008484, s, 10);	// 在背景层打印鼠标坐标信息
					sheetSlide(sheetMouse, mx, my);
				} 			
			} else if (bufval == 10) {	// 10秒定时器中断信息
				putFont8AscSheet(sheetBack, 0, 80, COL8_FFFFFF, COL8_008484,  "10[sec]", 7);	// 在背景层打印10秒提示	
				taskSwitch4();	// 切换至任务B
				// sprintf(s, "%010d", count);
				// putFont8AscSheet(sheetWin, 40, 28, COL8_000000, COL8_C6C6C6,  s, 11);	// 在窗口层打印count的值
			} else if (bufval == 3) {	// 3秒定时器中断信息
				putFont8AscSheet(sheetBack, 0, 96, COL8_FFFFFF, COL8_008484,  "3[sec]", 6);	// 在背景层打印3秒提示	
				// count = 0;
			} else if (bufval <= 1) {
				if (bufval == 1) {
					timerInit(timer3, &queue, 0);	// 超时信息置0
					cursorC = COL8_000000;	
				} else {
					timerInit(timer3, &queue, 1);	// 超时信息置1
					cursorC = COL8_FFFFFF;
				}
				boxFill8(sheetWin->buf, sheetWin->width, cursorC, cursorX, 28, cursorX + 7, 43);
				timerSetTime(timer3, 50);	// 设置超时时限0.5秒
				sheetRefresh(sheetWin, cursorX, 28, cursorX + 8, 44);
			}
		}
	}
}
