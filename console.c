#ifndef CONSOLE_C
#define CONSOLE_C
/********************************************************************************
* @File name: console.c
* @Author: suvvm
* @Version: 0.0.5
* @Date: 2020-02-07
* @Description: 实现控制台相关函数
********************************************************************************/

#include "bootpack.h"
#include "multiProcess.c"

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
			putFont8AscSheet(console->sheet, console->cursorX, console->cursorY, COL8_FFFFFF, COL8_000000, " ", 1);
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
		putFont8AscSheet(console->sheet, console->cursorX, console->cursorY, COL8_FFFFFF, COL8_000000, s, 1);
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
	sprintf(s, "memory %dMB free : %dKB", memsegTotalCnt / (1024 * 1024), memsegTotal(memsegtable) / 1024);
	putFont8AscSheet(console->sheet, 8, console->cursorY, COL8_FFFFFF, COL8_000000, s, 30);
	consoleNewLine(console);
	consoleNewLine(console);
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
				sprintf(s, "fileName.ext    %7d", fileInfo[i].size);
				for (j = 0; j < 8; j++) {	// 文件名
					s[j] = fileInfo[i].name[j];
				}
				for (j = 0; j < 3; j++) {	// 扩展名
					s[j + 9] = fileInfo[i].ext[j];
				}
				putFont8AscSheet(console->sheet, 8, console->cursorY, COL8_FFFFFF, COL8_000000, s, 30);
				consoleNewLine(console);
			}
		}
	}
	consoleNewLine(console);
}

/*******************************************************
*
* Function name: cmdType
* Description: 显示文件内容指令
* Parameter:
*	@console		控制台信息指针		struct CONSOLE *
*	@cmdline		指令信息			char *
*	@fat			内存fat表记录指针	int *
*
**********************************************************/
void cmdType(struct CONSOLE *console, int *fat, char *cmdline) {
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;	// 内存段表指针
	struct FILEINFO *fileInfo = searchFile(cmdline + 5, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);	// 根据文件名找到对应文件信息
	char *p;
	int i;
	if (fileInfo != 0) {	// 文件存在
		p = (char *) memsegAlloc4K(memsegtable, fileInfo->size);	// 为文件在内存中分配缓冲区
		loadFile(fileInfo->clusterNum, fileInfo->size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));	// 读取文件内容至内存缓冲区
		for (i = 0; i < fileInfo->size; i++) {	// 输出文件内容
			consolePutchar(console, p[i], 1);
		}
		memsegFree4K(memsegtable, (int) p, fileInfo->size);	// 释放缓冲区
	} else {	// 文件不存在
		putFont8AscSheet(console->sheet, 8, console->cursorY, COL8_FFFFFF, COL8_000000, "file not found", 14);
		consoleNewLine(console);
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
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;	// 段描述符表GDT地址为 0x270000~0x27ffff
	char name[18], *p;
	int i;
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
		setSegmdesc(gdt + 1003, fileInfo->size - 1, (int) p, AR_CODE32_ER);
		farCall(0, 1003 * 8);	// 调用另一段的函数
		memsegFree4K(memsegtable, (int) p, fileInfo->size);	// 释放文件缓冲区内存
		consoleNewLine(console);
		return 1;
	}
	return 0;
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
	if (strcmp(cmdline, "mem") == 0) {	// mem内存检测指令
		cmdMem(console, memsegTotalCnt);
	} else if (strcmp(cmdline, "cls") == 0) {	// cls清屏指令
		cmdCls(console);
	} else if (strcmp(cmdline, "dir") == 0) {	// dir显示目录文件信息指令
		cmdDir(console);
	} else if (strncmp(cmdline, "type ", 5) == 0) {	// type 显示文件内容指令
		cmdType(console, fat, cmdline);
	} else if (cmdline[0] != 0) {	// 不是指令 不是空行
		if (cmdApp(console, fat, cmdline) == 0) {	// 不是应用程序
			putFont8AscSheet(console->sheet, 8, console->cursorY, COL8_FFFFFF, COL8_000000, "command not found", 17);
			consoleNewLine(console);
			consoleNewLine(console);
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
	struct TIMER * timer;	// 定时器
	struct PCB *process = processNow();	// 取得当前占有处理机的进程
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;
	int bufval, buf[128], *fat = (int *) memsegAlloc4K(memsegtable, 4 * 2880);
	struct CONSOLE console;
	char cmdline[30];
	// 初始化控制台信息
	console.sheet = sheet;
	console.cursorY = 28;
	console.cursorX = 8;
	console.cursorC = -1;
	*((int *) 0x0fec) = (int) &console;	// 将console信息存入内存指定位置0x0fec
	
	QueueInit(&process->queue, 128, buf, process);	// 初始化缓冲区队列
	timer = timerAlloc();
	timerInit(timer, &process->queue, 1);
	timerSetTime(timer, 50);	// 0.5秒超时
	readFat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));	// 在磁盘对应地址解压fat数据至对应内存地址
	
	consolePutchar(&console, '>', 1); // 打印提示符
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
					if (console.cursorC >= 0) {	// 光标处于显示状态
						console.cursorC = COL8_FFFFFF;	// 光标白色
					}
				} else {
					timerInit(timer, &process->queue, 1);
					if (console.cursorC >= 0) {	// 光标处于显示状态
						console.cursorC = COL8_000000;	// 光标黑色
					}
				}
				timerSetTime(timer, 50);	
			}
			if (bufval == 2) {	// 由进程A写入的显示光标通知
				console.cursorC = COL8_FFFFFF;
			}
			if (bufval == 3) {	// 由进程A写入的隐藏光标通知
				boxFill8(sheet->buf,  sheet->width, COL8_000000, console.cursorX, console.cursorY, console.cursorX + 7, console.cursorY + 15);
				console.cursorC = -1;
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
					consolePutchar(&console, '>', 1);	// 打印提示符	
				} else {	// 普通字符
					if (console.cursorX < 240) {
						cmdline[console.cursorX / 8 - 2] = bufval - 256;
						consolePutchar(&console, bufval - 256, 1);
					}
				}
			}
			// 重新显示光标
			if (console.cursorC >= 0) {
				boxFill8(sheet->buf, sheet->width, console.cursorC, console.cursorX, console.cursorY, console.cursorX + 7, console.cursorY + 15);	// 重新绘制光标
			}
			sheetRefresh(sheet, console.cursorX, console.cursorY, console.cursorX + 8, console.cursorY + 16);
		}
	}
}

#endif	// CONSOLE_C
