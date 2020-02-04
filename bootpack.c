/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 0.4.1
* @Date: 2020-02-04
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
* Function name: consoleMain
* Description: 控制台进程主函数
*
**********************************************************/
void consoleMain(struct SHEET *sheet) {
	struct QUEUE queue;	// 缓冲区队列
	struct TIMER * timer;	// 定时器
	struct PCB *process = processNow();	// 取得当前占有处理机的进程
	
	int bufval, buf[128], cursorX = 8, cursorC = COL8_000000;	// 缓冲区字符值 缓冲区 光标x轴位置 光标颜色
	
	QueueInit(&queue, 128, buf, process);	// 初始化缓冲区队列
	timer = timerAlloc();
	timerInit(timer, &queue, 1);
	timerSetTime(timer, 50);	// 0.5秒超时
	
	for(;;) {
		io_cli();
		if (QueueSize(&queue) == 0) {
			processSleep(process);	// 无用时进程休眠
			io_sti();	// 开中断
		} else {
			bufval = QueuePop(&queue);
			io_sti();	// 开中断
			if (bufval <= 1) {	// 光标定时器超时
				if (bufval != 0) {
					timerInit(timer, &queue, 0);
					cursorC = COL8_FFFFFF;	// 光标白色
				} else {
					timerInit(timer, &queue, 1);
					cursorC = COL8_000000;	// 光标黑色
				}
				timerSetTime(timer, 50);
				boxFill8(sheet->buf, sheet->width, cursorC, cursorX, 28, cursorX + 7, 43);	// 重新绘制光标
				sheetRefresh(sheet, cursorX, 28, cursorX + 8, 44);
				
			}
		}
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
*	@act	活动标识		char
*
**********************************************************/
void makeWindow(unsigned char *buf, int width, int height, char *title, char act) {
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
	char c, tc, tbc;	// closeBtn对应位置字符 title文字颜色 窗口标题栏颜色
	if (act != 0) {	// 窗口为活动窗口
		tc = COL8_FFFFFF;	// 白色
		tbc = COL8_000084;	// 暗青色
	} else {	// 窗口不为活动窗口
		tc = COL8_C6C6C6;	// 亮灰色
		tbc = COL8_848484;	// 暗灰色
	}
	// 绘制窗口（坐标都为相对坐标，起始(0,0)）
	boxFill8(buf, width, COL8_C6C6C6, 0, 0, width - 1, 0);	// 亮灰色 (0,0)~(width-1,0)顶部横线
	boxFill8(buf, width, COL8_FFFFFF, 1, 1, width - 2, 1);	// 白色 (1,1)~(width-2,1)顶部横线
	boxFill8(buf, width, COL8_C6C6C6, 0, 0, 0, height - 1);	// 亮灰色 (0,0)~(0,height-1)左侧竖线
	boxFill8(buf, width, COL8_FFFFFF, 1, 1, 1, height - 2);	// 白色 (1,1)~(1,height-2)左侧竖线
	boxFill8(buf, width, COL8_848484, width - 2, 1, width - 2, height - 2);	//暗灰色 (width - 2,1)~(width - 2, height - 2)一条右侧竖线
	boxFill8(buf, width, COL8_000000, width - 1, 0, width - 1, height - 1);	// 黑色 (width - 1,0)~(width - 1, height - 1)一条右侧竖线
	boxFill8(buf, width, COL8_C6C6C6, 2, 2, width - 3, height - 3);	// 亮灰色 (2,2)~(width - 3,height - 3) 中心矩形
	boxFill8(buf, width, tbc, 3, 3, width - 4, 20);	// tbc(3,3)~(width - 4, 20)顶部矩形
	boxFill8(buf, width, COL8_848484, 1, height - 2, width - 2, height - 2);	// 暗灰色(1,height-2)~(width-2,height-2)底部横线
	boxFill8(buf, width, COL8_000000, 0, height - 1, width - 1, height - 1);	// 黑色(0,height-1)~(width-1,height-1)底部横线
	putFont8_asc(buf, width, 24, 4, tc, title);
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
	int mx, my, bufval, cursorX, cursorC; //鼠标x轴位置 鼠标y轴位置 光标x轴位置 光标颜色
	struct MouseDec mdec;	// 保存鼠标信息
	unsigned int memtotal;
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;	// 内存段表指针
	struct SHTCTL *shtctl;	// 图层控制块指针
	struct SHEET *sheetBack, *sheetMouse, *sheetWin, *sheetCons;	// 背景图层 鼠标图层 窗口图层 控制台图层
	unsigned char *bufBack, bufMouse[256], *bufWin, *bufCons;	// 背景图像缓冲区 鼠标图像缓冲区 窗口图像缓冲区 控制台图像缓冲区
	struct QUEUE queue;	// 总缓冲区
	struct TIMER *timer;	// 四个定时器指针
	struct PCB *processA, *processConsole;
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;	// 获取启动信息
	
	initGdtit();	// 初始化GDT IDT
	init_pic();	// 初始化可编程中断控制器
	io_sti();	// 解除cpu中断禁止
		
	QueueInit(&queue, 128, buf, 0);	// 初始化缓冲区队列
	
	initPit();	// 初始化定时器
	initKeyboard(&queue, 256);	// 初始化键盘控制电路
	enableMouse(&queue, 512, &mdec);	// 激活鼠标
	
	io_out8(PIC0_IMR, 0xf8); // 主PIC IRQ0(定时器) IRQ1（键盘）与IRQ2（从PIC）不被屏蔽(11111000)
	io_out8(PIC1_IMR, 0xef); // 从PIC IRQ12（鼠标）不被控制(11101111)

	timer = timerAlloc();	// 获取一个可使用的定时器
	timerInit(timer, &queue, 1);	// 初始化定时器1
	timerSetTime(timer, 50);
	
	memtotal = memtest(0x00400000, 0xbfffffff);	// 获取内存总和
	memsegInit(memsegtable);
	memsegFree(memsegtable, 0x00001000, 0x0009e000);
	memsegFree(memsegtable, 0x00400000, memtotal - 0x00400000);
	
	init_palette();	// 初始化16色调色板
	
	shtctl = shtctlInit(memsegtable, binfo->vram, binfo->scrnx, binfo->scrny);
	
	processA = processInit(memsegtable);	// 多进程初始化
	queue.process = processA;
	processRun(processA, 1, 0);	// 进程A处于第1级
	
	sheetBack = sheetAlloc(shtctl);
	bufBack = (unsigned char *) memsegAlloc4K(memsegtable, binfo->scrnx * binfo->scrny);	// 以4KB为单位为背景分配内存
	sheetSetbuf(sheetBack, bufBack, binfo->scrnx, binfo->scrny, -1);	// 设置背景图层缓冲区，背景不需要透明色设为-1
	init_GUI(bufBack, binfo->scrnx, binfo->scrny);	// 初始化GUI至bufBack
	
	sheetCons = sheetAlloc(shtctl);	// 为控制台分配图层
	bufCons = (unsigned char *) memsegAlloc4K(memsegtable, 256 * 165);
	sheetSetbuf(sheetCons, bufCons, 256, 165, -1);	// 设置控制台图层缓冲区，控制台不需要透明色
	makeWindow(bufCons, 256, 165, "console", 0);	// 创建控制台窗口
	makeTextBox(sheetCons, 8, 28, 240, 128, COL8_000000);	// 创建控制台黑色输入框
	
	processConsole = processAlloc();	// 分配控制台进程
	processConsole->tss.esp = memsegAlloc4K(memsegtable, 64 * 1024) + 64 * 1024 - 8;	// 为控制台进程栈分配64KB内存并计算栈底地址给B的ESP 为了下方传值时不超内存范围 这里减去8	
	*((int *) (processConsole->tss.esp + 4)) = (int) sheetCons;	// 将sheetCons地址存入内存地址 esp + 4 c语言函数指定的参数在ESP+4的位置
	
	processConsole->tss.eip = (int) &consoleMain;	// 控制台进程的下一条指令执行consoleMain
	
	processConsole->tss.es = 1 * 8;
	processConsole->tss.cs = 2 * 8;
	processConsole->tss.ss = 1 * 8;
	processConsole->tss.ds = 1 * 8;
	processConsole->tss.fs = 1 * 8;
	processConsole->tss.gs = 1 * 8;
	
	processRun(processConsole, 2, 2);	
	// 控制台进程 level2 2 进入就绪队列
	
	sheetWin = sheetAlloc(shtctl);
	bufWin = (unsigned char *) memsegAlloc4K(memsegtable, 160 * 52);
	sheetSetbuf(sheetWin, bufWin, 144, 52, -1);	// 设置窗口图层缓冲区 无透明色
	makeWindow(bufWin, 144, 52, "processA", 1);	// 显示窗口
	makeTextBox(sheetWin, 8, 28, 128, 16, COL8_FFFFFF);	// 显示文本输入区
	
	cursorX = 8;	// 设置光标起始位置
	cursorC = COL8_FFFFFF;	// 设置光标颜色
	sheetMouse = sheetAlloc(shtctl);
	sheetSetbuf(sheetMouse, bufMouse, 16, 16, COL8_008484);	// 设置鼠标图层缓冲区与透明色
	initMouseCursor8(bufMouse, COL8_008484);	// 初始化鼠标至bufMouse
	// 初始鼠标坐标为屏幕正中
	mx = (binfo->scrnx - 16) / 2;	// 鼠标x轴位置
	my = (binfo->scrny - 28 - 16) / 2;	// 鼠标y轴位置
	
	sheetSlide(sheetBack, 0, 0);	// 背景图层起始坐标(0,0)
	sheetSlide(sheetCons, 168, 64);	// 控制台进程窗口起始坐标(168, 56)
	sheetSlide(sheetWin, 8, 64);	// 进程A 窗口起始坐标(8, 56);
	sheetSlide(sheetMouse, mx, my);	// 将鼠标图层滑动至对应位置
	
	sheetUpdown(sheetBack, 0);	// 将背景图层置于索引0
	sheetUpdown(sheetCons, 1);	// 将控制台进程窗口图层置于索引1
	sheetUpdown(sheetWin, 2);	// 将进程A窗口图层置于置于索引4
	sheetUpdown(sheetMouse, 3);	// 将鼠标图层置于索引5
	
	putFont8AscSheet(sheetBack, 0, 32, COL8_FFFFFF, COL8_008484,  "Welcome to DickOS", 17);	// 将DickOS写入背景层
	sprintf(s, "(%3d, %3d)", mx, my);	// 将鼠标位置存入s
	putFont8AscSheet(sheetBack, 0, 0, COL8_FFFFFF, COL8_008484,  s, 10);	// 将s写入背景层
	sprintf(s, "memory %dMB free : %dKB", memtest(0x00400000, 0xbfffffff) / (1024 * 1024), memsegTotal(memsegtable) / 1024);	// 将内存信息存入s
	putFont8AscSheet(sheetBack, 0, 48, COL8_FFFFFF, COL8_008484,  s, 26);	// 将s写入背景层
	
	//处理中断与进入hlt
	
	for(;;){
		io_cli();	// 关中断
		if(QueueSize(&queue) == 0) {	// 只有缓冲区没有数据时才能开启中断
			processSleep(processA);	
			// 进程A休眠 若A不休眠则永远不会调度至进程B
			io_sti();	// 开中断
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
			} else if (bufval <= 1) {
				if (bufval == 1) {
					timerInit(timer, &queue, 0);	// 超时信息置0
					cursorC = COL8_000000;	
				} else {
					timerInit(timer, &queue, 1);	// 超时信息置1
					cursorC = COL8_FFFFFF;
				}
				boxFill8(sheetWin->buf, sheetWin->width, cursorC, cursorX, 28, cursorX + 7, 43);
				timerSetTime(timer, 50);	// 设置超时时限0.5秒
				sheetRefresh(sheetWin, cursorX, 28, cursorX + 8, 44);
			}
		}
	}
}
