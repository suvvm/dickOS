/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 0.5.5
* @Date: 2020-02-05
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

/*******************************************************
*
* Function name: consNewLine
* Description: 控制台转入新行
* Parameter:
*	@cursorY	光标y轴位置	int
*	@sheet		图层指针	struct SHEET *
* Return:
*	返回光标y轴新的位置
*
**********************************************************/
int consNewLine(int cursorY, struct SHEET *sheet) {
	int x, y;
	if (cursorY < 28 + 112) {	// 未触底不需要滚动
		cursorY += 16;
	} else {	// 滚动
		for (y = 28; y < 28 + 112; y ++) {	// 将第二行至最后一行上移
			for (x = 8; x < 8 + 240; x ++) {	
				sheet->buf[x + y * sheet->width] = sheet->buf[x + (y + 16) * sheet->width];
			}
		}
		for (y = 28 + 112; y < 28 + 128; y ++) {	// 将最后一行绘制为黑色
			for (x = 8; x < 8 + 240; x ++) {
				sheet->buf[x + y * sheet->width] = COL8_000000;
			}
		}
		sheetRefresh(sheet, 8, 28, 8 + 240, 28 + 128);
	}
	return cursorY;
}


/********************************************************
*
* Function name: consoleMain
* Description: 控制台进程主函数
*
**********************************************************/
void consoleMain(struct SHEET *sheet, unsigned int memsegTotalCnt) {
	struct TIMER * timer;	// 定时器
	struct PCB *process = processNow();	// 取得当前占有处理机的进程
	int bufval, buf[128], cursorX = 16, cursorY = 28, cursorC = -1, x, y;	// 缓冲区字符值 缓冲区 光标x轴位置 光标颜色-1为不显示光标
	char s[30], cmdline[30], *p;
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;
	struct FILEINFO *fileInfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);	// 读取fat16根目录
	
	QueueInit(&process->queue, 128, buf, process);	// 初始化缓冲区队列
	timer = timerAlloc();
	timerInit(timer, &process->queue, 1);
	timerSetTime(timer, 50);	// 0.5秒超时
	
	putFont8AscSheet(sheet, 8, 28, COL8_FFFFFF, COL8_000000, ">", 1);	// 打印提示符
	for(;;) {
		io_cli();
		if (QueueSize(&process->queue) == 0) {
			processSleep(process);	// 无用时进程休眠
			io_sti();	// 开中断
		} else {
			bufval = QueuePop(&process->queue);
			io_sti();	// 开中断
			if (bufval <= 1) {	// 光标定时器超时
				if (bufval != 0) {
					timerInit(timer, &process->queue, 0);
					if (cursorC >= 0) {	// 光标处于显示状态
						cursorC = COL8_FFFFFF;	// 光标白色
					}
				} else {
					timerInit(timer, &process->queue, 1);
					if (cursorC >= 0) {	// 光标处于显示状态
						cursorC = COL8_000000;	// 光标黑色
					}
				}
				timerSetTime(timer, 50);	
			}
			if (bufval == 2) {	// 由进程A写入的显示光标通知
				cursorC = COL8_FFFFFF;
			}
			if (bufval == 3) {	// 由进程A写入的隐藏光标通知
				boxFill8(sheet->buf,  sheet->width, COL8_000000, cursorX, cursorY, cursorX + 7, cursorY + 15);
				cursorC = -1;
			}
			if (256 < bufval && bufval <= 511) {	// 键盘数据
				if (bufval == 8 + 256) {	// 退格
					if (cursorX > 16) {
						putFont8AscSheet(sheet, cursorX, cursorY, COL8_FFFFFF, COL8_000000, " ", 1);
						cursorX -= 8;
					}
				} else if (bufval == 10 + 256) {	// 回车
					putFont8AscSheet(sheet, cursorX, cursorY, COL8_FFFFFF, COL8_000000, " ", 1);	// 用空格消除光标
					cmdline[cursorX / 8 - 2] = 0;
					cursorY = consNewLine(cursorY, sheet);
					if (strcmp(cmdline, "mem") == 0) {	// mem命令
						sprintf(s, "memory %dMB free : %dKB", memsegTotalCnt / (1024 * 1024), memsegTotal(memsegtable) / 1024);
						putFont8AscSheet(sheet, 8, cursorY, COL8_FFFFFF, COL8_000000, s, 30);
						cursorY = consNewLine(cursorY, sheet);
						cursorY = consNewLine(cursorY, sheet);
					} else if (strcmp(cmdline, "cls") == 0) {	// cls命令
						for (y = 28; y < 28 + 128; y++) {
							for (x = 8; x < 8 + 240; x++) {
								sheet->buf[x + y * sheet->width] = COL8_000000;
							}
						}
						sheetRefresh(sheet, 8, 28, 8 + 240, 28 + 128);
						cursorY = 28;
					}else if (strcmp(cmdline, "dir") == 0) {	// dir命令
						for (x = 0; x < 244; x++) {	// 遍历所有文件信息
							if (fileInfo[x].name[0] == 0x00) {	// 不包含任何文件信息
								break;
							}
							if (fileInfo[x].name[0] != 0xe5) {	// 文件未被删除
								if ((fileInfo[x].type & 0x18) == 0) {	// 非目录信息
									sprintf(s, "fileName.ext    %7d", fileInfo[x].size);
									for (y = 0; y < 8; y++) {	// 文件名
										s[y] = fileInfo[x].name[y];
									}
									for (y = 0; y < 3; y++) {	// 扩展名
										s[y + 9] = fileInfo[x].ext[y];
									}
									putFont8AscSheet(sheet, 8, cursorY, COL8_FFFFFF, COL8_000000, s, 30);
									cursorY = consNewLine(cursorY, sheet);
								}
							}
						}
						cursorY = consNewLine(cursorY, sheet);
					} else if (strncmp(cmdline, "type ", 5) == 0) {
						for (y = 0; y < 11; y++) {
							s[y] = ' ';
						}
						y = 0;
						for (x = 5; y < 11 && cmdline[x] != 0; x++) {	// 获取文件名转为与fat16根目录文件名相同格式存入s
							if (cmdline[x] == '.' && y <= 8) {	// 找到文件名中的.
								y = 8;	// 开始准备获取扩展名
							} else {
								s[y] = cmdline[x];
								if ('a' <= s[y] && s[y] <= 'z') {	// 小写字母转大写
									s[y] -= 0x20;
								}
								y++;
							}
						}
						// 寻找文件
						for (x = 0; x < 244;) {
							if (fileInfo[x].name[0] == 0x00) {	// 不包含任何文件信息
								break;
							}
							char flag = 0;
							if ((fileInfo[x].type & 0x18) == 0) { // 不为目录
								for (y = 0; y < 11; y++) {
									if (fileInfo[x].name[y] != s[y]) {
										flag = 1;
										break;
									}
								}
							}
							if (flag == 1) {
								x++;
								continue;
							}
							break;
						}
						if (x < 224 && fileInfo[x].name[0] != 0x00) {
							y = fileInfo[x].size;
							p = (char *) (fileInfo[x].clusterNum * 512 + 0x003e00 + ADR_DISKIMG);	// 文件在磁盘中的地址 = clusterNum * 512（一个扇区） + 0x003e00
							cursorX = 8;
							for (x = 0; x < y; x++) {	// 逐字打印
								s[0] = p[x];
								s[1] = 0;
								if (s[0] == 0x09) {	// 制表符
									for (;;) {
										putFont8AscSheet(sheet, cursorX, cursorY, COL8_FFFFFF, COL8_000000, " ", 1);
										cursorX += 8;
										if (cursorX == 8 + 240) {
											cursorX = 8;
											cursorY = consNewLine(cursorY, sheet);
										}
										if(((cursorX - 8) & 0x1f) == 0) {	// 一个制表符将会填充空格至当前行字符数量为4的倍数 一个字符8个像素 4 * 8 = 32
											break;
										}
									}
								} else if (s[0] == 0x0a) { // 换行
									cursorX = 8;
									cursorY = consNewLine(cursorY, sheet);
								} else if (s[0] == 0x0d) {	// 回车
									// 暂无操作
								}else {	// 一般字符
									putFont8AscSheet(sheet, cursorX, cursorY, COL8_FFFFFF, COL8_000000, s, 1);
									cursorX += 8;
									if (cursorX == 8 + 240) {
										cursorX = 8;
										cursorY =consNewLine(cursorY, sheet);
									}
								}
							}
						} else {
							putFont8AscSheet(sheet, 8, cursorY, COL8_FFFFFF, COL8_000000, "file not found", 14);
							cursorY = consNewLine(cursorY, sheet);
						}
						cursorY = consNewLine(cursorY, sheet);
						
					} else if (cmdline[0] != 0) {
						// 不是空行也不是命令
						putFont8AscSheet(sheet, 8, cursorY, COL8_FFFFFF, COL8_000000, "command not found", 17);
						cursorY = consNewLine(cursorY, sheet);
						cursorY = consNewLine(cursorY, sheet);
					}
					
					putFont8AscSheet(sheet, 8, cursorY, COL8_FFFFFF, COL8_000000, ">", 1);
					cursorX = 16;	
				} else {	// 普通字符
					if (cursorX < 240) {
						s[0] = bufval - 256;
						s[1] = 0;
						cmdline[cursorX / 8 - 2] = bufval - 256; 
						putFont8AscSheet(sheet, cursorX, cursorY, COL8_FFFFFF, COL8_000000, s, 1);
						cursorX += 8;
					}
				}
			}
			// 重新显示光标
			if (cursorC >= 0) {
				boxFill8(sheet->buf, sheet->width, cursorC, cursorX, cursorY, cursorX + 7, cursorY + 15);	// 重新绘制光标
			}
			sheetRefresh(sheet, cursorX, cursorY, cursorX + 8, cursorY + 16);
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
* Function name: makeWindowTitle
* Description: 绘制窗口标题栏
* Parameter:
*	@buf	窗口图像缓冲区	unsigned char *
*	@width	窗口宽度		int
*	@title	窗口标题		char *
*	@act	活动标识		char
*
**********************************************************/
void makeWindowTitle(unsigned char *buf, int width, char *title, char act) {
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
	boxFill8(buf, width, tbc, 3, 3, width - 4, 20);	// tbc(3,3)~(width - 4, 20)标题矩形
	putFont8_asc(buf, width, 24, 4, tc, title);	// 标题文字
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
	
	// 绘制窗口（坐标都为相对坐标，起始(0,0)）
	boxFill8(buf, width, COL8_C6C6C6, 0, 0, width - 1, 0);	// 亮灰色 (0,0)~(width-1,0)顶部横线
	boxFill8(buf, width, COL8_FFFFFF, 1, 1, width - 2, 1);	// 白色 (1,1)~(width-2,1)顶部横线
	boxFill8(buf, width, COL8_C6C6C6, 0, 0, 0, height - 1);	// 亮灰色 (0,0)~(0,height-1)左侧竖线
	boxFill8(buf, width, COL8_FFFFFF, 1, 1, 1, height - 2);	// 白色 (1,1)~(1,height-2)左侧竖线
	boxFill8(buf, width, COL8_848484, width - 2, 1, width - 2, height - 2);	//暗灰色 (width - 2,1)~(width - 2, height - 2)一条右侧竖线
	boxFill8(buf, width, COL8_000000, width - 1, 0, width - 1, height - 1);	// 黑色 (width - 1,0)~(width - 1, height - 1)一条右侧竖线
	boxFill8(buf, width, COL8_C6C6C6, 2, 2, width - 3, height - 3);	// 亮灰色 (2,2)~(width - 3,height - 3) 中心矩形
	boxFill8(buf, width, COL8_848484, 1, height - 2, width - 2, height - 2);	// 暗灰色(1,height-2)~(width-2,height-2)底部横线
	boxFill8(buf, width, COL8_000000, 0, height - 1, width - 1, height - 1);	// 黑色(0,height-1)~(width-1,height-1)底部横线
	makeWindowTitle(buf, width, title, act);
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
	int	buf[128], keyCmdBuf[32];	// s保存要输出的变量信息 buf为总缓冲区
	int mx, my, bufval, cursorX, cursorC; //鼠标x轴位置 鼠标y轴位置 光标x轴位置 光标颜色
	int keyShift = 0, keyTo = 0, keyLeds, keyCmdWait = -1;	// shift按下标识	活动窗口标识
	struct MouseDec mdec;	// 保存鼠标信息
	unsigned int memtotal;
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;	// 内存段表指针
	struct SHTCTL *shtctl;	// 图层控制块指针
	struct SHEET *sheetBack, *sheetMouse, *sheetWin, *sheetCons;	// 背景图层 鼠标图层 窗口图层 控制台图层
	unsigned char *bufBack, bufMouse[256], *bufWin, *bufCons;	// 背景图像缓冲区 鼠标图像缓冲区 窗口图像缓冲区 控制台图像缓冲区
	struct QUEUE queue, keyCmd;	// 总缓冲区 存储欲向键盘控制电路发送的数据的缓冲区
	struct TIMER *timer;	// 四个定时器指针
	struct PCB *processA, *processConsole;
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;	// 获取启动信息
	keyLeds = (binfo->leds >> 4) & 7;	// 获取键盘各锁定键状态 binfo->leds的4~6位
	initGdtit();	// 初始化GDT IDT
	init_pic();	// 初始化可编程中断控制器
	io_sti();	// 解除cpu中断禁止
		
	QueueInit(&queue, 128, buf, 0);	// 初始化缓冲区队列
	
	initPit();	// 初始化定时器
	initKeyboard(&queue, 256);	// 初始化键盘控制电路
	enableMouse(&queue, 512, &mdec);	// 激活鼠标
	
	io_out8(PIC0_IMR, 0xf8); // 主PIC IRQ0(定时器) IRQ1（键盘）与IRQ2（从PIC）不被屏蔽(11111000)
	io_out8(PIC1_IMR, 0xef); // 从PIC IRQ12（鼠标）不被控制(11101111)
	QueueInit(&keyCmd, 32, keyCmdBuf, 0);

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
	processConsole->tss.esp = memsegAlloc4K(memsegtable, 64 * 1024) + 64 * 1024 - 12;	// 为控制台进程栈分配64KB内存并计算栈底地址给B的ESP 为了下方传值时不超内存范围 这里减去8	
	*((int *) (processConsole->tss.esp + 4)) = (int) sheetCons;	// 将sheetCons地址存入内存地址 esp + 4 c语言函数指定的参数在ESP+4的位置
	*((int *) (processConsole->tss.esp + 8)) = memtotal;
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
	/*
	sprintf(s, "memory %dMB free : %dKB", memtest(0x00400000, 0xbfffffff) / (1024 * 1024), memsegTotal(memsegtable) / 1024);	// 将内存信息存入s
	putFont8AscSheet(sheetBack, 0, 48, COL8_FFFFFF, COL8_008484,  s, 26);	// 将s写入背景层
	*/
	// 为了避免和键盘当前状态冲突，在一开始先进行设置指示灯状态
	QueuePush(&keyCmd, KEYCMD_LED);	// 0xed
	QueuePush(&keyCmd, keyLeds);
	for(;;){
		if (QueueSize(&keyCmd) > 0 && keyCmdWait < 0) {	// 如果存在要向键盘控制器发送的数据就发送它
			keyCmdWait = QueuePop(&keyCmd);
			waitKeyboardControllerReady();
			io_out8(PORT_KEYDAT, keyCmdWait);	// 向键盘控制电路发送
		}
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
					if (keyShift == 0) {	// 并未按下shift
						s[0] = keyboardTable0[bufval - 256];
					} else {	// shift正在按下
						s[0] = keyboardTable1[bufval - 256];
					}
				} else {
					s[0] = 0;
				}
				
				if ('A' <= s[0] && s[0] <= 'Z') {	// s[0]为英文字母
					if (((keyLeds & 4) == 0 && keyShift == 0) || ((keyLeds & 4) != 0 && keyShift != 0)) {	// 需要转换为小写
						s[0] += 0x20;
					}
				}
				
				if (s[0] != 0) {	// 普通字符
					if (keyTo == 0) {	// processA 窗口
						if (cursorX < 128) {
							s[1] = 0;
							putFont8AscSheet(sheetWin, cursorX, 28, COL8_000000, COL8_FFFFFF, s, 1);	// 将字符打印至窗口层
							cursorX += 8;	// 光标后移一个字符
						}
					} else {
						QueuePush(&processConsole->queue, s[0] + 256);
					}
				}
				if (bufval == 256 + 0x0e) {	// 按下退格键且光标不在输入框起始位置
					if (keyTo == 0) {
						if (cursorX > 8) {
							putFont8AscSheet(sheetWin, cursorX, 28, COL8_000000, COL8_FFFFFF, " ", 1);	// 用空格消去当前光标
							cursorX -= 8;	// 光标前移一个字符
						}
					} else {
						QueuePush(&processConsole->queue, 8 + 256);
					}
					
				}
				if (bufval == 256 + 0x0f) {	// 按下tab键
					if (keyTo == 0) {
						keyTo = 1;
						makeWindowTitle(bufWin, sheetWin->width, "processA", 0);
						makeWindowTitle(bufCons, sheetCons->width, "console", 1);
						cursorC = -1;	// 不显示光标
						boxFill8(sheetWin->buf, sheetWin->width, COL8_FFFFFF, cursorX, 28, cursorX + 7, 43);
						QueuePush(&processConsole->queue, 2);	// 向控制台进程缓冲区写入3 通知其显示光标
					} else {
						keyTo = 0;
						makeWindowTitle(bufWin, sheetWin->width, "processA", 1);
						makeWindowTitle(bufCons, sheetCons->width, "console", 0);
						cursorC = COL8_000000;	// 显示光标
						QueuePush(&processConsole->queue, 3);	// 向控制台进程缓冲区写入2 通知其隐藏光标
					}
					sheetRefresh(sheetWin, 0, 0, sheetWin->width, 21);
					sheetRefresh(sheetCons, 0, 0, sheetCons->width, 21);
				}
				if (bufval == 256 + 0x2a) {	// 左shift按下
					keyShift |= 1;
				}
				if (bufval == 256 + 0x36) {	// 右shift按下
					keyShift |= 2;
				}
				if (bufval == 256 + 0xaa) {	// 左shift抬起
					keyShift &= ~1;
				}
				if (bufval == 256 + 0xb6) {	// 右shift抬起
					keyShift &= ~2;
				}
				if (bufval == 256 + 0x3a) {	// CapsLock
					keyLeds ^= 4;	// 改变keyLeds对应位
					QueuePush(&keyCmd, KEYCMD_LED);
					QueuePush(&keyCmd, keyLeds);
					// 将0xed keyLeds存入缓冲区等待向键盘控制电路发送
				}
				if (bufval == 256 + 0x45) {	// NumLock
					keyLeds ^= 2;	// 改变keyLeds对应位
					QueuePush(&keyCmd, KEYCMD_LED);
					QueuePush(&keyCmd, keyLeds);
					// 将0xed keyLeds存入缓冲区等待向键盘控制电路发送
				}
				if (bufval == 256 + 0x46) {	// ScrollLock
					keyLeds ^= 1;	// 改变keyLeds对应位
					QueuePush(&keyCmd, KEYCMD_LED);
					QueuePush(&keyCmd, keyLeds);
					// 将0xed keyLeds存入缓冲区等待向键盘控制电路发送
				}
				if (bufval == 256 + 0xfa) {	// 键盘控制电路返回成功数据
					keyCmdWait = -1; // 等待进行下一次发送
				}
				if (bufval == 256 + 0xfe) {	// 键盘控制电路返回失败数据
					waitKeyboardControllerReady();	// 等待键盘控制电路就绪
					io_out8(PORT_KEYDAT, keyCmdWait);	// 重新向键盘控制电路发送8位数据
				}
				if (bufval == 256 + 0x1c) {	// 回车
					if (keyTo != 0) {	// 发送至控制台窗口
						QueuePush(&processConsole->queue, 10 + 256);
					}
				}
				// 重新显示光标
				if (cursorC > 0) {
					boxFill8(sheetWin->buf, sheetWin->width, cursorC, cursorX, 28, cursorX + 7, 43);
				}
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
					if (cursorC >= 0) {
						cursorC = COL8_000000;
					}
				} else {
					timerInit(timer, &queue, 1);	// 超时信息置1
					if (cursorC >= 0) {
						cursorC = COL8_FFFFFF;
					}
				}
				timerSetTime(timer, 50);	// 设置超时时限0.5秒
				if (cursorC >= 0) {
					boxFill8(sheetWin->buf, sheetWin->width, cursorC, cursorX, 28, cursorX + 7, 43);
					sheetRefresh(sheetWin, cursorX, 28, cursorX + 8, 44);
				}
			}
		}
	}
}
