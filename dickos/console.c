#ifndef CONSOLE_C
#define CONSOLE_C
/********************************************************************************
* @File name: console.c
* @Author: suvvm
* @Version: 0.2.1
* @Date: 2020-02-14
* @Description: 实现控制台相关函数
********************************************************************************/

#include "bootpack.h"
#include "window.c"
#include "multiProcess.c"

/*******************************************************
*
* Function name: openConsole
* Description: 创建新的不显示窗口的控制台进程
* Parameter:
*	@shtctl		图层控信息指针	struct SHTCTL *
*	@memtotal	内存总量		unsigned int
* Return:
*	返回控制台进程控制块指针
*
**********************************************************/
struct PCB *openConsoleProcess(struct SHEET *sheet, unsigned int memtotal) {
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;	// 内存段表指针
	struct PCB *process = processAlloc();	// 分配控制台进程
	int *consBuf = (int *) memsegAlloc4K(memsegtable, 128 * 4);
	process->stack = memsegAlloc4K(memsegtable, 64 * 1024);	// 为控制台进程栈分配64KB内存
	process->tss.esp = process->stack + 64 * 1024 - 12;	// 计算栈底地址给B的ESP 为了下方传值时不超内存范围 这里减去8	
	*((int *) (process->tss.esp + 4)) = (int) sheet;	// 将sheet地址存入内存地址 esp + 4 c语言函数指定的参数在ESP+4的位置
	*((int *) (process->tss.esp + 8)) = memtotal;
	process->tss.eip = (int) &consoleMain;	// 控制台进程的下一条指令执行consoleMain
		
	process->tss.es = 1 * 8;
	process->tss.cs = 2 * 8;
	process->tss.ss = 1 * 8;
	process->tss.ds = 1 * 8;
	process->tss.fs = 1 * 8;
	process->tss.gs = 1 * 8;
	
	processRun(process, 2, 2);
	// 控制台进程 level2 2 进入就绪队列
	QueueInit(&process->queue, 128, consBuf, process);
	return process;
}

/*******************************************************
*
* Function name: openConsole
* Description: 创建新的控制台
* Parameter:
*	@shtctl		图层控信息指针	struct SHTCTL *
*	@memtotal	内存总量		unsigned int
* Return:
*	返回控制台图层指针
*
**********************************************************/
struct SHEET *openConsole(struct SHTCTL *shtctl, unsigned int memtotal) {
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;	// 内存段表指针
	struct SHEET *sheet = sheetAlloc(shtctl);
	unsigned char *buf = (unsigned char *) memsegAlloc4K(memsegtable, 256 * 165);
	sheetSetbuf(sheet, buf, 256, 165, -1);	// 无透明色
	
	makeWindow(buf, 256, 165, "console", 0);	// 创建控制台窗口
	makeTextBox(sheet, 8, 28, 240, 128, COL8_000000);	// 创建控制台黑色输入框	
	
	// 控制台进程 level2 2 进入就绪队列
	sheet->process = openConsoleProcess(sheet, memtotal);
	sheet->status |= 0x20;	// 有光标	（status 0x10位标识窗口是否由应用程序生成 0x20位判断是否需要光标）
	return sheet;
}

/*******************************************************
*
* Function name: closeConsoleProcess
* Description: 关闭控制台进程
* Parameter:
*	@process	控制台进程控制块	struct PCB *
*
*********************************************************/
void closeConsoleProcess(struct PCB *process) {
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;	// 内存段表指针
	processSleep(process);
	memsegFree4K(memsegtable, process->stack, 64 * 1024);	// 释放进程栈
	memsegFree4K(memsegtable, (int) process->queue.buf, 128 * 4);	// 释放进程缓冲区
	process->status = 0;
}

/*******************************************************
*
* Function name: closeConsole
* Description: 关闭控制台
* Parameter:
*	@sheet	控制台图层指针	struct SHEET *
*
*********************************************************/
void closeConsole(struct SHEET *sheet) {
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;	// 内存段表指针
	memsegFree4K(memsegtable, (int) sheet->buf, 256 * 165);	// 释放控制台图层缓冲区
	sheetFree(sheet);
	closeConsoleProcess(sheet->process);
}

/*******************************************************
*
* Function name: consoleNewLine
* Description: 控制台转入新行
* Parameter:
*	@console	控制台信息指针	struct CONSOLE *
*
**********************************************************/
void consoleNewLine(struct CONSOLE *console) {
	int x, y;
	struct SHEET *sheet = console->sheet;
	if (console->cursorY < 28 + 112) { // 未触底不需要滚动
		console->cursorY += 16;
	} else {
		if (sheet != 0) {
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
	}
	console->cursorX = 8;
}

/*******************************************************
*
* Function name: consolePutchar
* Description: 控制台打印字符
* Parameter:
*	@console	控制台信息指针			struct CONSOLE *
*	@chr		欲打印字符编号(ascii码)	int
*	@move		记录是否需要后移光标	char
*
**********************************************************/
void consolePutchar(struct CONSOLE *console, int chr, char move) {
	char s[2];
	s[0] = chr;
	s[1] = 0;
	if (s[0] == 0x09) {	// 制表符
		for (;;) {
			if (console->sheet != 0) {
				putFont8AscSheet(console->sheet, console->cursorX, console->cursorY, COL8_FFFFFF, COL8_000000, " ", 1);
			}
			console->cursorX += 8;
			if (console->cursorX == 8 + 240) {	// 到控制台一行的末端换行
				consoleNewLine(console);
			}
			if(((console->cursorX - 8) & 0x1f) == 0) {	// 一个制表符将会填充空格至当前行字符数量为4的倍数 一个字符8个像素 4 * 8 = 32
				break;
			}
		}
	} else if (s[0] == 0x0a) { // 换行
		consoleNewLine(console);
	} else if (s[0] == 0x0d) {	// 回车
		// 暂无操作
	}else {	// 一般字符
		if (console->sheet != 0) {
			putFont8AscSheet(console->sheet, console->cursorX, console->cursorY, COL8_FFFFFF, COL8_000000, s, 1);	
		}
		if (move != 0) {	// move为0时光标不后移动
			console->cursorX += 8;
			if (console->cursorX == 8 + 240) {
				consoleNewLine(console);
			}
		}
	}
}

/*******************************************************
*
* Function name: consolePutstr0
* Description: 输出字符串遇0结束
* Parameter:
*	@console	控制台信息指针	struct CONSOLE *
*	@str		欲输出的字符串	char *
*
**********************************************************/
void consolePutstr0(struct CONSOLE *console, char *str) {
	for (; *str != 0; str++) {	// 遇0截止
		consolePutchar(console, *str, 1);
	}
}

/*******************************************************
*
* Function name: consolePutstr1
* Description: 输出指定长度字符串
* Parameter:
*	@console	控制台信息指针	struct CONSOLE *
*	@str		欲输出的字符串	char *
*	@len		欲出输的长度	int
*
**********************************************************/
void consolePutstr1(struct CONSOLE *console, char *str, int len) {
	int i;
	for (i = 0; i < len; i++) {	// 遇0截止
		consolePutchar(console, str[i], 1);
	}
}

/*******************************************************
*
* Function name: cmdMem
* Description: 检查内存指令
* Parameter:
*	@console		控制台信息指针		struct CONSOLE *
*	@memsegTotalCnt	内存大小总和		unsigned int
*
**********************************************************/
void cmdMem(struct CONSOLE *console, unsigned int memsegTotalCnt) {
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;	// 内存段表指针
	char s[30];
	sprintf(s, "memory %dMB free : %dKB\n\n", memsegTotalCnt / (1024 * 1024), memsegTotal(memsegtable) / 1024);
	consolePutstr0(console, s);
}

/*******************************************************
*
* Function name: cmdCls
* Description: 清屏指令
* Parameter:
*	@console		控制台信息指针		struct CONSOLE *
*
**********************************************************/
void cmdCls(struct CONSOLE *console) {
	int x, y;
	for (y = 28; y < 28 + 128; y++) {
		for (x = 8; x < 8 + 240; x++) {
			console->sheet->buf[x + y * console->sheet->width] = COL8_000000;
		}
	}
	sheetRefresh(console->sheet, 8, 28, 8 + 240, 28 + 128);
	console->cursorY = 28;
}

/*******************************************************
*
* Function name: cmdDir
* Description: 显示当前目录文件信息指令
* Parameter:
*	@console		控制台信息指针		struct CONSOLE *
*
**********************************************************/
void cmdDir(struct CONSOLE *console) {
	int i, j;
	struct FILEINFO *fileInfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);	// 读取fat16根目录
	char s[30];
	for (i = 0; i < 244; i++) {	// 遍历所有文件信息
		if (fileInfo[i].name[0] == 0x00) {	// 不包含任何文件信息
			break;
		}
		if (fileInfo[i].name[0] != 0xe5) {	// 文件未被删除
			if ((fileInfo[i].type & 0x18) == 0) {	// 非目录信息
				sprintf(s, "fileName.ext    %7d\n", fileInfo[i].size);
				for (j = 0; j < 8; j++) {	// 文件名
					s[j] = fileInfo[i].name[j];
				}
				for (j = 0; j < 3; j++) {	// 扩展名
					s[j + 9] = fileInfo[i].ext[j];
				}
				consolePutstr0(console, s);
			}
		}
	}
	consoleNewLine(console);
}

/*******************************************************
*
* Function name: cmdApp
* Description: 执行应用程序的指令
* Parameter:
*	@console		控制台信息指针		struct CONSOLE *
*	@fat			内存fat表记录指针	int *
*	@cmdline		指令信息			char *
* Return:
*	成功返回1 失败返回0 
*
**********************************************************/
int cmdApp(struct CONSOLE *console, int *fat, char *cmdline) {
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;	// 内存段表指针
	struct FILEINFO *fileInfo;
	struct PCB *process = processNow();
	struct SHTCTL *shtctl;
	struct SHEET *sheet;
	char name[18], *p, *q;
	int i, segsiz, datasiz, esp, dathrb;
	for(i = 0; i < 13; i++) {
		if (cmdline[i] <= ' ') {
			break;
		}
		name[i] = cmdline[i];
	}
	name[i] = 0;
	fileInfo = searchFile(name , (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);	// 根据文件名找到对应文件信息
	if (fileInfo == 0 && name[i - 1] != '.') {	// 为文件添加后缀
		name[i] = '.';
		name[i + 1] = 'H';
		name[i + 2] = 'R';
		name[i + 3] = 'B';
		name[i + 4] = 0;
		fileInfo = searchFile(name , (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);	// 根据文件名找到对应文件信息
	}
	
	if (fileInfo != 0) {	// 文件存在
		p = (char *) memsegAlloc4K(memsegtable, fileInfo->size);	// 为文件在内存中分配缓冲区
		loadFile(fileInfo->clusterNum, fileInfo->size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));	// 读取文件内容至内存缓冲区
		if (fileInfo->size >= 36 && strncmp(p + 4, "Hari", 4) == 0 && *p == 0x00) {	// 该文件为应用程序
			segsiz	= *((int *) (p + 0x0000));	// 获取操作系统应为应用程序准备的数据段的大小
			esp 	= *((int *) (p + 0x000c));	// 获取ESP初始值和应用程序数据部分要传送的目的地址
			datasiz	= *((int *) (p + 0x0010));	// 获取hrb文件内数据部分的大小
			dathrb	= *((int *) (p + 0x0014));	// 获取hrb文件数据部分开始的位置
			q = (char *) memsegAlloc4K(memsegtable, segsiz);	// 为应用程序分配专属内存空间
			process->dsBase = (int) q;
			//*((int *) 0xfe8) = (int) q;	
			// 将q的地址存入内存0xfe8的位置以供dickApi使用
			setSegmdesc(process->ldt + 0, fileInfo->size - 1, (int) p, AR_CODE32_ER + 0x60);	// 注册应用程序代码段至该进程局部描述符表 +0x60将段设置为应用程序 应用程序若想写入操作系统的段地址就会发生异常
			setSegmdesc(process->ldt + 1, segsiz - 1, (int) q, AR_DATA32_RW + 0x60);	// 注册应用程序运行段至该进程局部描述符表
			for (i = 0; i < datasiz; i++) {	// 将数据部分送至目的地址
				q[esp + i] = p[dathrb + i];
			}
			startApp(0x1b, 0 * 8 + 4, esp, 1 * 8 + 4, &(process->tss.esp0));	// 启动应用程序并设置ESP与DS.SS
			// 应用程序结束后
			shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
			for (i = 0; i < MAX_SHEETS; i++) {	// 遍历所有图层找到属于该应用程序进程的图层并关闭
				sheet = &(shtctl->sheets[i]);
				if ((sheet->status & 0x11) == 0x11 && sheet->process == process) {
					sheetFree(sheet);
				}
			}
			for (i = 0; i < 8; i++) {	// 将未关闭的文件关闭
				if (process->fileHandle[i].buf != 0) {
					memsegFree4K(memsegtable, (int) process->fileHandle[i].buf, process->fileHandle[i].size);	// 释放文件缓冲区
					process->fileHandle[i].buf = 0;	// 标记为未使用
				}
			}
			timerCancelAllFlags(&process->queue);
			memsegFree4K(memsegtable, (int) q, segsiz);	// 释放应用程序专有内存
		} else {
			consolePutstr0(console, ".hrb file format error\n");
		}
		memsegFree4K(memsegtable, (int) p, fileInfo->size);	// 释放文件缓冲区内存
		consoleNewLine(console);
		return 1;
	}
	return 0;
}

/*******************************************************
*
* Function name: cmdExit
* Description: 退出控制台
* Parameter:
*	@console		控制台信息指针		struct CONSOLE *
*	@fat			内存fat表记录指针	int *
*
**********************************************************/
void cmdExit(struct CONSOLE *console, int *fat) {
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;	// 内存段表指针
	struct PCB *process = processNow();
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct QUEUE *queue = (struct QUEUE *) *((int *) 0x0fec);
	timerCancle(console->timer);	// 取消光标定时器
	memsegFree4K(memsegtable, (int) fat, 4 * 2880);	// 释放内存中的fat表
	io_cli();
	if (console->sheet != 0) {
		QueuePush(queue, console->sheet - shtctl->sheets + 768);	// 向主进程发送数据 768 ~ 1023 
	} else {
		QueuePush(queue, process - processctl->processes + 1024);	// 向主进程发送数据 1024 ~ 2023 
	}
	io_sti();
	for(;;) {
		processSleep(process);
	}
}

/*******************************************************
*
* Function name: cmdStart
* Description: 打开新的控制台并在新的控制台中执行目标指令
* Parameter:
*	@console		控制台信息指针		struct CONSOLE *
*	@cmdline		指令信息			char *
*	@memtotal		内存容量大小		int
*
**********************************************************/
void cmdStart(struct CONSOLE *console, char *cmdline, int memtotal) {
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);	// 获取图层控制信息
	struct SHEET *sheet = openConsole(shtctl, memtotal);	// 打开新的控制台窗口
	struct QUEUE *queue = &sheet->process->queue;
	int i;
	sheetSlide(sheet, 32, 4);
	sheetUpdown(sheet, shtctl->top);
	for (i = 6; cmdline[i] != 0; i++) {
		QueuePush(queue, cmdline[i] + 256);
	}
	QueuePush(queue, 10 + 256);	// 回车
	consoleNewLine(console);
	
}

/*******************************************************
*
* Function name: cmdNcst
* Description: 不打开新的控制台执行目标指令
* Parameter:
*	@console		控制台信息指针		struct CONSOLE *
*	@cmdline		指令信息			char *
*	@memtotal		内存容量大小		int
*
**********************************************************/
void cmdNcst(struct CONSOLE *console, char *cmdline, int memtotal) {
	struct PCB *process = openConsoleProcess(0, memtotal);	// 打开新的控制台窗口
	struct QUEUE *queue = &process->queue;
	int i;
	for (i = 5; cmdline[i] != 0; i++) {
		QueuePush(queue, cmdline[i] + 256);
	}
	QueuePush(queue, 10 + 256);	// 回车
	consoleNewLine(console);
}

/*******************************************************
*
* Function name: consoleRunCmd
* Description: 控制台执行指令
* Parameter:
*	@cmdline		指令信息			char *
*	@console		控制台信息指针		struct CONSOLE *
*	@fat			内存fat表记录指针	int *
*	@memsegTotalCnt	内存大小总和		unsigned int
*
**********************************************************/
void consoleRunCmd(char *cmdline, struct CONSOLE * console, int *fat, unsigned int memsegTotalCnt) {
	if (strcmp(cmdline, "mem") == 0 && console->sheet != 0) {	// mem内存检测指令
		cmdMem(console, memsegTotalCnt);
	} else if (strcmp(cmdline, "cls") == 0 && console->sheet != 0) {	// cls清屏指令
		cmdCls(console);
	} else if (strcmp(cmdline, "dir") == 0 && console->sheet != 0) {	// dir显示目录文件信息指令
		cmdDir(console);
	} else if (strcmp(cmdline, "exit") == 0) {
		cmdExit(console, fat);
	} else if (strncmp(cmdline, "start ", 6) == 0) {
		cmdStart(console, cmdline, memsegTotalCnt);
	} else if (strncmp(cmdline, "ncst ", 5) == 0){
		cmdNcst(console, cmdline, memsegTotalCnt);
	} else if (cmdline[0] != 0) {	// 不是指令 不是空行
		if (cmdApp(console, fat, cmdline) == 0) {	// 不是应用程序
			consolePutstr0(console, "command not found\n\n");
		}
	}
}

/********************************************************
*
* Function name: consoleMain
* Description: 控制台进程主函数
*
**********************************************************/
void consoleMain(struct SHEET *sheet, unsigned int memsegTotalCnt) {
	struct PCB *process = processNow();	// 取得当前占有处理机的进程
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;
	int bufval, *fat = (int *) memsegAlloc4K(memsegtable, 4 * 2880), i;
	struct CONSOLE console;
	struct FILEHANDLE fileHandle[8];
	char cmdline[30];
	// 初始化控制台信息
	console.sheet = sheet;
	console.cursorY = 28;
	console.cursorX = 8;
	console.cursorC = -1;
	process->console = &console;
	process->cmdline = cmdline;
	//*((int *) 0x0fec) = (int) &console;	
	// 将console信息存入内存指定位置0x0fec
	
	if (console.sheet != 0) {
		console.timer = timerAlloc();
		timerInit(console.timer, &process->queue, 1);
		timerSetTime(console.timer, 50);	// 0.5秒超时		
	}
	readFat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));	// 在磁盘对应地址解压fat数据至对应内存地址
	
	for (i = 0; i < 8; i++) {
		fileHandle[i].buf = 0;	// 未使用
	}
	process->fileHandle = fileHandle;
	process->fat = fat;
	
	consolePutchar(&console, '>', 1); // 打印提示符
	for(;;) {
		io_cli();
		if (QueueSize(&process->queue) == 0) {
			processSleep(process);	// 无用时进程休眠
			io_sti();	// 开中断
		} else {
			bufval = QueuePop(&process->queue);
			io_sti();	// 开中断
			if (bufval <= 1 && console.sheet != 0) {	// 光标定时器超时
				if (bufval != 0) {
					timerInit(console.timer, &process->queue, 0);
					if (console.cursorC >= 0) {	// 光标处于显示状态
						console.cursorC = COL8_FFFFFF;	// 光标白色
					}
				} else {
					timerInit(console.timer, &process->queue, 1);
					if (console.cursorC >= 0) {	// 光标处于显示状态
						console.cursorC = COL8_000000;	// 光标黑色
					}
				}
				timerSetTime(console.timer, 50);	
			}
			if (bufval == 2) {	// 由进程A写入的显示光标通知
				console.cursorC = COL8_FFFFFF;
			}
			if (bufval == 3) {	// 由进程A写入的隐藏光标通知
				if (console.sheet != 0) {
					boxFill8(console.sheet->buf, console.sheet->width, COL8_000000, console.cursorX, console.cursorY, console.cursorX + 7, console.cursorY + 15);
				}
				console.cursorC = -1;
			}
			if (bufval == 4) {
				cmdExit(&console, fat);
			}
			if (256 < bufval && bufval <= 511) {	// 键盘数据
				if (bufval == 8 + 256) {	// 退格
					if (console.cursorX > 16) {
						// putFont8AscSheet(sheet, cursorX, cursorY, COL8_FFFFFF, COL8_000000, " ", 1);
						consolePutchar(&console, ' ', 0);
						console.cursorX -= 8;
					}
				} else if (bufval == 10 + 256) {	// 回车
					consolePutchar(&console, ' ', 0);
					cmdline[console.cursorX / 8 - 2] = 0;
					consoleNewLine(&console);
					consoleRunCmd(cmdline, &console, fat, memsegTotalCnt);	// 执行命令行语句
					if (console.sheet == 0) {
						cmdExit(&console, fat);
					}
					consolePutchar(&console, '>', 1);	// 打印提示符	
				} else {	// 普通字符
					if (console.cursorX < 240) {
						cmdline[console.cursorX / 8 - 2] = bufval - 256;
						consolePutchar(&console, bufval - 256, 1);
					}
				}
			}
			// 重新显示光标
			if (console.sheet != 0) {
				if (console.cursorC >= 0) {
					boxFill8(console.sheet->buf, console.sheet->width, console.cursorC, console.cursorX, console.cursorY, console.cursorX + 7, console.cursorY + 15);	// 重新绘制光标
				}
				sheetRefresh(console.sheet, console.cursorX, console.cursorY, console.cursorX + 8, console.cursorY + 16);
			}			
		}
	}
}

/*******************************************************
*
* Function name: dickApi
* Description: dickOS系统调用api
* Parameter: 按PUSHAD的顺序接收寄存器的值
*	@edi	int
*	@esi	int
*	@ebp	int
*	@esp	int
*	@ebx	int
*	@edx	int
*	@ecx	int
*	@eax	int
*
**********************************************************/
int *dickApi(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax) {
	struct PCB *process = processNow();
	int dsBase = process->dsBase;	// 获取应用程序文件所在内存地址
	struct CONSOLE *console = process->console;
	//struct CONSOLE *console = (struct CONSOLE *) *((int *) 0x0fec);	
	// 在指定内存地址获取控制台信息
	struct SHTCTL *shtctl = (struct SHTCTL *)  *((int *)0x0fe4);	// 在指定内存读取图层控制信息
	struct SHEET *sheet;
	struct QUEUE *sysQueue = (struct QUEUE *) *((int *) 0x0fec);
	struct FILEINFO *fileInfo;
	struct FILEHANDLE *fileHandle;
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;
	int *reg = &eax + 1; // 两次pushad 找到第一次pushad就可以修改先前保存的寄存器的值
	int bufval, i;
	
	if (edx == 1) {	// 功能号1 显示单个字符
		consolePutchar(console, eax & 0xff, 1); 	// AL中存放字符ascii码
	} else if (edx == 2) {	// 功能号2 显示字符串到0为止
		consolePutstr0(console, (char *) ebx + dsBase);	// ebx  + dsBase中存放msg字符串首地址
	} else if (edx == 3) {	// 功能号3 显示指定长度字符串
		consolePutstr1(console, (char *) ebx + dsBase, ecx);	// ebx  + dsBase中存放msg字符串首地址 ecx中存放长度
	} else if (edx == 4) {	// 功能号4 结束应用程序
		return &(process->tss.esp0);
	} else if (edx == 5) {	// 功能号5 显示窗口
		sheet = sheetAlloc(shtctl);
		sheet->process = process;
		sheet->status |= 0x10;	// 标记为应用程序窗口
		sheetSetbuf(sheet, (char *) ebx + dsBase, esi, edi, eax); // 缓冲区地址为ebx + dsBase 宽度esi 高度edi 透明色号 eax
		makeWindow((char *) ebx + dsBase, esi, edi, (char *)ecx + dsBase, 0);	// 缓冲区ebx + dsBase 宽度esi 高度edi 窗口标题首位地址 ecx+dsBase 非活动窗口
		sheetSlide(sheet, ((shtctl->xSize - esi) / 2) & ~3, (shtctl->ySize - edi) / 2); // 使窗口在x坐标为4的倍数以加速图层刷新速度
		sheetUpdown(sheet, shtctl->top);
		reg[7] = (int) sheet;	// 将先前保存的EAX寄存器的值更换为sheet
	} else if (edx == 6) {	// 功能号6 在图层中显示字符
		sheet = (struct SHEET *) (ebx & 0xfffffffe);	// 图层句柄
		putFont8_asc(sheet->buf, sheet->width, esi, edi, eax, (char *)ebp + dsBase);	// 缓冲区sheet->buf 宽度sheet->width x轴起始位置esi y轴起始位置edi 颜色eax 字符串首地址ebp + dsBase
		if ((ebx & 1) == 0) {	// 奇数不刷新
			sheetRefresh(sheet, esi, edi, esi + ecx * 8, edi + 16);	// 图层sheet x轴起始位置esi y轴起始位置edi 字数ecx 高度16
		}
	} else if (edx == 7) {	// 功能号7 在图层中绘制矩形
		sheet = (struct SHEET *) (ebx & 0xfffffffe);	// 图层句柄
		boxFill8(sheet->buf, sheet->width, ebp, eax, ecx, esi, edi);	// 颜色ebp x轴起始位置eax y轴起始位置ecx x轴截止位置esi y轴截止位置edi
		if ((ebx & 1) == 0) {	// 奇数不刷新
			sheetRefresh(sheet, eax, ecx, esi + 1, edi + 1);
		}
	} else if (edx == 8) {	// 功能号8 memseg初始化
		memsegInit((struct MEMSEGTABLE *) (ebx + dsBase));	// 内存段表 ebx + dsBase
		ecx &= 0xfffffff0; // 以16字节为单位
		memsegFree((struct MEMSEGTABLE *) (ebx + dsBase), eax, ecx);	// 释放首地址eax 偏移量ecx
	} else if (edx == 9) {	// 功能号9 malloc 分配内存
		ecx = (ecx + 0x0f) & 0xfffffff0;	// 以16字节为单位
		reg[7] = memsegAlloc((struct MEMSEGTABLE *) (ebx + dsBase), ecx);	// 将先前保存的EAX寄存器的值更换为分配的首地址 分配大小为ecx
	} else if (edx == 10) { // 功能号10 free 释放内存
		ecx = (ecx + 0x0f) & 0xfffffff0;	// 以16字节为单位
		memsegFree((struct MEMSEGTABLE *) (ebx + dsBase), eax, ecx);	// 释放首地址eax 偏移量ecx
	} else if (edx == 11) {	// 功能号11 绘制1 * 1小矩形实现绘制像素点功能
		sheet = (struct SHEET *) (ebx & 0xfffffffe);	// 图层句柄
		sheet->buf[sheet->width * edi + esi] = eax;	// x轴起始位置esi y轴起始位置edi 颜色eax
		if ((ebx & 1) == 0) { // 奇数不刷新
			sheetRefresh(sheet, esi, edi, esi + 1, edi + 1); 	// 刷新目标像素
		}
	} else if (edx == 12) {	// 功能号12 刷新图层
		sheet = (struct SHEET *) ebx;
		sheetRefresh(sheet, eax, ecx, esi, edi);	// x轴起始位置eax y轴起始位置ecx x轴截止位置esi y轴截止位置edi	
	} else if (edx == 13) {	// 功能号13 绘制直线
		sheet = (struct SHEET *) (ebx & 0xfffffffe);
		dickApiLineWin(sheet, eax, ecx, esi, edi, ebp);	// 绘制直线 x轴起始位置eax y轴起始位置ecx x轴截止位置esi y轴截止位置edi 色号ebp
		if ((ebx & 1) == 0) {
			sheetRefresh(sheet, eax, ecx, esi + 1, edi + 1);
		}
	} else if (edx == 14) {	// 功能号14 关闭窗口
		sheetFree((struct SHEET *) ebx);
	} else if (edx == 15) {	// 功能号15 接收键盘输入数据与定时器中断数据
		for (;;) {
			io_cli();	// 关中断
			if (QueueSize(&process->queue) == 0) {
				if (eax != 0) {
					processSleep(process);	// 缓冲区为空休眠等待
				} else {
					io_sti();
					reg[7] = -1;	// 修改先前保存的eax为-1
					return 0;
				}
			}
			bufval = QueuePop(&process->queue);
			io_sti();	// 开中断
			if (bufval <= 1) {	// 光标定时器中断数据
				timerInit(console->timer, &process->queue, 1);	// 应用程序运行时不需要显示光标 总是置1
				timerSetTime(console->timer, 50);
			}
			if (bufval == 2) {	// 显示光标通知
				console->cursorC = COL8_FFFFFF;
			}
			if (bufval == 3) {	// 隐藏光标通知
				console->cursorC = -1;
			}
			if (bufval == 4) {
				timerCancle(console->timer);
				io_cli();
				QueuePush(sysQueue, console->sheet - shtctl->sheets + 2024);
				console->sheet = 0;
				io_sti();
			}
			if (bufval >= 256) {	// 键盘数据与定时器超时数据（由进程A发生）
				reg[7] = bufval - 256;	// 将键盘数据信息存入先前保存的EAX中
				return 0;
			}
		}
	} else if (edx == 16) {	// 功能号16 定时器分配 alloc
		reg[7] = (int) timerAlloc();	// EAX设置为定时器句柄
		((struct TIMER *) reg[7])->flags = 1;	// 应用程序结束时自动取消
	} else if (edx == 17) {	// 功能号17 定时器初始化 init
		timerInit((struct TIMER *) ebx, &process->queue, eax + 256);	// 定时器句柄ebx 发送数据eax
	} else if (edx == 18) {	// 功能号18 设置定时器时间 set
		timerSetTime((struct TIMER *) ebx, eax);	// 定时器句柄ebx 时间eax
	} else if (edx == 19) {	// 功能号19 释放定时器 free
		timerFree((struct TIMER *) ebx);	// 定时器句柄ebx
	} else if (edx == 20) {	// 功能号20 控制蜂鸣器发声
		if (eax == 0) {	// eax声音频率0mHz
			i = io_in8(0x61);	// 在I/O端口0x61读入数据
			io_out8(0x61, i & 0x0d);	// 向I/O端口0x61发送数据使蜂鸣器关闭
		} else {
			// 设置音高
			i = 11938000 / eax;
			io_out8(0x43, 0xb6);
			
			io_out8(0x42, i & 0xff);	// 设定值低8位
			io_out8(0x42, i >> 8);		// 设定值高8位
			
			// 向I/O端口0x61发送数据使蜂鸣器开启
			i = io_in8(0x61);
			io_out8(0x61, (i | 0x03) & 0x0f);
		}
	} else if (edx == 21) {	// 功能号21 打开文件
		for (i = 0; i < 8; i++) {
			if (process->fileHandle[i].buf == 0) {	// 找到可用于存储文件句柄的位置
				break;
			}
		}
		fileHandle = &process->fileHandle[i];
		reg[7] = 0;	// EAX初始设置为0
		if (i < 8) {
			// 寻找文件信息	EBX文件名
			fileInfo = searchFile((char *) ebx + dsBase, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
			if (fileInfo != 0) {	// 找到文件
				fileHandle->buf = (char *) memsegAlloc4K(memsegtable, fileInfo->size);
				// 为文件分配内存
				fileHandle->size = fileInfo->size;
				// 记录文件大小
				fileHandle->pos = 0;
				// 文件当前读取位置为文件首
				
				loadFile(fileInfo->clusterNum, fileInfo->size, fileHandle->buf, process->fat, (char *) (ADR_DISKIMG + 0x003e00));	// 加载文件至内存缓冲区
				reg[7] = (int) fileHandle;	// 文件句柄存入寄存器EAX
			}
		}
	} else if (edx == 22) {	// 功能号22 关闭文件
		fileHandle = (struct FILEHANDLE *) eax;	// eax为文件句柄
		memsegFree4K(memsegtable, (int) fileHandle->buf, fileHandle->size);	// 释放文件内存缓冲区
		fileHandle->buf = 0;	// 未使用
	} else if (edx == 23) {	// 功能号23 文件定位
		fileHandle = (struct FILEHANDLE *) eax;	// eax为文件句柄
		// ecx 定位模式 ebx定位偏移量
		if (ecx == 0) {	// 起点为文件开头
			fileHandle->pos = ebx;
		} else if (ecx == 1) {	// 定位起点为当前访问位置
			fileHandle->pos += ebx;
		} else if (ecx == 2) {	// 定位起点为文件结尾
			fileHandle->pos = fileHandle->size + ebx;
		}
		if (fileHandle->pos < 0) {
			fileHandle->pos = 0;
		}
		if (fileHandle->pos > fileHandle->size) {
			fileHandle->pos = fileHandle->size;
		}
	} else if (edx == 24) {	// 功能号24 获取文件大小
		fileHandle = (struct FILEHANDLE *) eax;	// eax为文件句柄
		// ecx 文件大小获取模式
		if(ecx == 0) {	// 文件大小
			reg[7] = fileHandle->size;
		} else if (ecx == 1) {	// 文件首到当前位置的大小
			reg[7] = fileHandle->pos;
		} else if (ecx == 2) {	// 文件当前位置到文件末尾的大小
			reg[7] = fileHandle->pos - fileHandle->size;
		} 
	} else if (edx == 25) {	// 功能号25 文件读取
		fileHandle = (struct FILEHANDLE *) eax;	// eax为文件句柄
		// ecx 最大读取字节数
		for (i = 0; i < ecx; i++) {
			if (fileHandle->pos == fileHandle->size) {	// 已读至文件尾
				break;
			}
			// ebx 要写入的缓冲区地址
			*((char *) ebx + dsBase + i) = fileHandle->buf[fileHandle->pos];
			fileHandle->pos++;
		}
		reg[7] = i;
	} else if (edx == 26) { // 功能号26 接收控制台输入的指令信息
		i = 0;
		for (;;) {
			*((char *) ebx + dsBase + i) = process->cmdline[i];	// 将cmdline第i位存指内存指定位置
			if (process->cmdline[i] == 0) {	// cmdline全部存完
				break;
			}
			if (i >= ecx) {	// 超出最大存放字节数
				break;
			}
			i++;
		}
		reg[7] = i;	// 将最终存放的字节数写入EAX
	}
	return 0;
}

/*******************************************************
*
* Function name: dickApiLineWin
* Description: 在图层中绘制直线
* Parameter: 
*	@sheet	图层指针	struct SHEET *
*	@startX	x轴起始位置	int
*	@startY	y轴起始位置 int
*	@endX	x轴截止位置	int
*	@endY	y轴截止位置	int
*	@col	色号		int
*
**********************************************************/
void dickApiLineWin(struct SHEET * sheet, int startX, int startY, int endX, int endY, int col) {
	int i, x, y, len, dx, dy;
	dx = endX - startX;	// x偏移量
	dy = endY - startY;	// y偏移量
	x = startX << 10;	// startX * 1024
	y = startY << 10;	// startY * 1024
	if (dx < 0) {
		dx = -dx;
	}
	if (dy < 0) {
		dy = -dy;
	}
	if (dx >= dy) {
		len = dx + 1;
		if (startX > endX) {
			dx = -1024;
		} else {
			dx = 1024;
		}
		if (startY <= endY) {
			dy = ((endY - startY + 1) << 10) / len;
		} else {
			dy = ((endY - startY - 1) << 10) / len;
		}
	} else {
		len = dy + 1;
		if (startY > endY) {
			dy = -1024;
		} else {
			dy = 1024;
		}
		if (startX <= endX) {
			dx = ((endX - startX + 1) << 10) / len;
		} else {
			dx = ((endX - startX - 1) << 10) / len;
		}
	}
	for (i = 0; i < len; i++) {
		sheet->buf[(y >> 10) * sheet->width + (x >> 10)] = col;
		x += dx;
		y += dy;
	}
}

#endif	// CONSOLE_C
