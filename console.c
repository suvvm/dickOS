#ifndef CONSOLE_C
#define CONSOLE_C
/********************************************************************************
* @File name: console.c
* @Author: suvvm
* @Version: 0.0.2
* @Date: 2020-02-06
* @Description: 实现控制台相关函数
********************************************************************************/

#include "bootpack.h"

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
	int *fat = (int *) memsegAlloc4K(memsegtable, 4 * 2880);	// 在内存中为fat表分配空间
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;	// 段描述符表GDT地址为 0x270000~0x27ffff
	readFat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));	// 在磁盘对应地址解压fat数据至对应内存地址
	
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
						if (x < 224 && fileInfo[x].name[0] != 0x00) {	// 找到文件
							p = (char *) memsegAlloc4K(memsegtable, fileInfo[x].size);	// 为文件在内存中分配缓冲区
							// 将磁盘中的文件读入内存
							loadFile(fileInfo[x].clusterNum , fileInfo[x].size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));	// 文件在磁盘中的地址 = clusterNum * 512（一个扇区） + 0x003e00
							cursorX = 8;
							for (y = 0; y < fileInfo[x].size; y++) {	// 逐字打印
								s[0] = p[y];
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
							memsegFree4K(memsegtable, (int) p, fileInfo[x].size);	// 释放文件缓冲区内存
						} else {
							putFont8AscSheet(sheet, 8, cursorY, COL8_FFFFFF, COL8_000000, "file not found", 14);
							cursorY = consNewLine(cursorY, sheet);
						}
						cursorY = consNewLine(cursorY, sheet);
						
					} else if (strcmp(cmdline, "clihlt") == 0) {
						for (y = 0; y < 11; y++) {
							s[y] = ' ';
						}
						s[0] = 'C', s[1] = 'L', s[2] = 'I';
						s[3] = 'H', s[4] = 'L', s[5] = 'T';
						s[8] = 'H', s[9] = 'R', s[10] = 'B';
						
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
						if (x < 224 && fileInfo[x].name[0] != 0x00) {	// 找到文件
							p = (char *) memsegAlloc4K(memsegtable, fileInfo[x].size);	// 为文件在内存中分配缓冲区
							// 将磁盘中的文件读入内存
							loadFile(fileInfo[x].clusterNum , fileInfo[x].size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));	// 文件在磁盘中的地址 = clusterNum * 512（一个扇区） + 0x003e00
							// 将读入的文件作为一个进程运行 在全局描述符表中定义这个进程状态段
							setSegmdesc(gdt + 1003, fileInfo[x].size - 1, (int) p, AR_CODE32_ER);
							farJmp(0,1003 * 8);	// 跳转至该进程
							memsegFree4K(memsegtable, (int) p, fileInfo[x].size);	// 释放文件缓冲区内存
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

#endif	// CONSOLE_C
