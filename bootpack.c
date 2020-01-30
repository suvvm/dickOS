/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 0.2.4
* @Date: 2020-01-30
* @Description: 包含启动后要使用的功能函数
********************************************************************************/
#include "bootpack.h"
#include "desctab.c"
#include "graphic.c"
#include "queue.h"
#include "mouse.c"
#include "memory.c"
#include "sheet.c"
#include "timer.c"

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
	char s[40], keyb[32], mouseb[128], timerb[8];	// s保存要输出的变量信息 keyb键盘缓冲区 mouseb鼠标缓冲区 timerb定时器缓冲区
	int mx, my, bufval; //鼠标x轴位置 鼠标y轴位置 要显示的缓冲区信息
	struct MouseDec mdec;	// 保存鼠标信息
	unsigned int memtotal, count = 0;
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;	// 内存段表指针
	struct SHTCTL *shtctl;	// 图层控制块指针
	struct SHEET *sheetBack, *sheetMouse, *sheetWin;	// 背景图层 鼠标图层 窗口图层
	unsigned char *bufBack, bufMouse[256], *bufWin;	// 背景图像缓冲区 鼠标图像缓冲区 窗口图像缓冲区
	struct QUEUE timerbuf;
	
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;	// 获取启动信息
	
	initGdtit();	// 初始化GDT IDT
	init_pic();	// 初始化可编程中断控制器
	io_sti();	// 解除cpu中断禁止
	
	
	QueueInit(&keybuf, 32, keyb);	//初始化键盘缓冲区队列
	QueueInit(&mousebuf, 128, mouseb);	// 初始化鼠标缓冲区队列
	
	
	
	initPit();	// 初始化定时器
	io_out8(PIC0_IMR, 0xf8); // 主PIC IRQ0(定时器) IRQ1（键盘）与IRQ2（从PIC）不被屏蔽(11111000)
	io_out8(PIC1_IMR, 0xef); // 从PIC IRQ12（鼠标）不被控制(11101111)
	
	QueueInit(&timerbuf, 8, timerb);	// 初始化定时器缓冲区队列
	setTimer(1000, &timerbuf, 1);	// 设置定时器信息 一定要在IRQ0中断开放后
	
	initKeyboard();
	enableMouse(&mdec);	// 激活鼠标
	
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
	makeWindow(bufWin, 160, 52, "counter");	// 显示窗口
	// putFont8_asc(bufWin, 160, 24, 28, COL8_000000, "Welcome to");
	// putFont8_asc(bufWin, 160, 24, 44, COL8_000000, "DickOS");
	sheetSlide(sheetBack, 0, 0);	// 背景图层起始坐标(0,0)
	// 初始鼠标坐标为屏幕正中
	mx = (binfo->scrnx - 16) / 2;	// 鼠标x轴位置
	my = (binfo->scrny - 28 - 16) / 2;	// 鼠标y轴位置
	sheetSlide(sheetMouse, mx, my);	// 将鼠标图层滑动至对应位置
	sheetSlide(sheetWin, 80, 80);	// 将窗口图层滑动至目标位置
	sheetUpdown(sheetBack, 0);	// 将背景图层置于索引0
	sheetUpdown(sheetWin, 1);	// 将窗口图层置于背景之上鼠标之下
	sheetUpdown(sheetMouse, 2);	// 将鼠标图层置于背景层上方索引1
	putFont8_asc(bufBack, binfo->scrnx, 30, 32, COL8_FFFFFF, "DickOS");	// 将DickOS写入背景层
	sprintf(s, "(%d, %d)", mx, my);	// 将鼠标位置存入s
	putFont8_asc(bufBack, binfo->scrnx, 0, 0, COL8_FFFFFF, s);	// 将s写入背景图层
	sprintf(s, "memory %dMB free : %dKB", memtest(0x00400000, 0xbfffffff) / (1024 * 1024), memsegTotal(memsegtable) / 1024);	// 将内存信息存入s
	putFont8_asc(bufBack, binfo->scrnx, 0, 64, COL8_FFFFFF, s);	// 将s写入背景图层
	sheetRefresh(sheetBack, 0, 0, binfo->scrnx, 64 + 16 + 16 + 76);	// 刷新图层
	
	//处理键盘与鼠标中断与进入hlt
	for(;;){
		sprintf(s, "%010d", timerctl.count);
		boxFill8(bufWin, 160, COL8_C6C6C6, 40, 28, 119, 43);
		putFont8_asc(bufWin, 160, 40, 28, COL8_000000, s);
		
		sheetRefresh(sheetWin, 40, 28, 120, 44);
		
		io_cli();	// 关中断
		if(QueueSize(&keybuf) + QueueSize(&mousebuf) + QueueSize(&timerbuf) == 0) {	// 只有在两个缓冲区都没有数据时才能开启中断并进入hlt模式
			io_sti();	// 开中断
		} else {
			if (QueueSize(&keybuf) != 0) {
				bufval = QueuePop(&keybuf);
				io_sti();
				sprintf(s, "%02X", bufval);
				boxFill8(bufBack, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
				putFont8_asc(bufBack, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
				sheetRefresh(sheetBack, 0, 16, 16, 32);
			} else if (QueueSize(&mousebuf) != 0) {
				bufval = QueuePop(&mousebuf);
				io_sti();
				if (mouseDecode(&mdec, bufval) != 0) {	//完成一波三个字节数据的接收或者出现未知差错
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					if ((mdec.btn & 0x01) != 0) {	// 按下左键
						s[1] = 'L';
					}
					if ((mdec.btn & 0x02) != 0) {	// 按下右键
						s[3] = 'R';
					}
					if ((mdec.btn & 0x04) != 0) {	// 中间滚轮
						s[2] = 'C';
					}
					
					boxFill8(bufBack, binfo->scrnx, COL8_008484, 32, 16, 32 + 15 * 8 - 1, 31);	// 用背景色覆盖原有鼠标信息
					putFont8_asc(bufBack, binfo->scrnx, 32, 16, COL8_FFFFFF, s);	//打印鼠标信息
					// 鼠标坐标加上偏移量
					sheetRefresh(sheetBack, 32, 16, 32 + 15 * 8, 32);
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
					boxFill8(bufBack, binfo->scrnx, COL8_008484, 0, 0, 79, 15);	// 覆盖原有坐标信息
					putFont8_asc(bufBack, binfo->scrnx, 0, 0, COL8_FFFFFF, s);	// 显示新的坐标信息
					sheetRefresh(sheetBack, 0, 0, 80, 16);
					sheetSlide(sheetMouse, mx, my);
				} 
			} else if (QueueSize(&timerbuf) != 0) {
				bufval = QueuePop(&timerbuf);	// 获取定时器缓冲区队列队首数据
				io_sti();	// 开中断
				putFont8_asc(bufBack, binfo->scrnx, 0, 80, COL8_FFFFFF, "10[sec]");
				sheetRefresh(sheetBack, 0, 64, 56, 64 + 16 + 16 + 76);
			}
		}
	}
}
