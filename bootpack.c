/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 0.6.2
* @Date: 2020-02-11
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
#include "window.c"
#include "console.c"
#include "file.c"

/*******************************************************
*
* Function name: keyWinOff
* Description: 使当前正在获取键盘数据的窗口停止获取键盘数据
* Parameter:
*	@keyWin		当前正在接收键盘数据的窗口指针				struct SHEET *
*	@sheetWin	主进程ProcessA输入框窗口					struct SHEET *
*	@cursorC	当前主进程ProcessA输入框窗口光标颜色状态	int
*	@cursorX	当前主进程ProcessA输入框窗口光标位置		int
* Return:
*	返回应为主进程processA中的输入框设置的光标颜色状态
*
**********************************************************/
int keyWinOff(struct SHEET *keyWin, struct SHEET *sheetWin, int cursorC, int cursorX) {
	changeWinTitle(keyWin, 0);	// 改变keyWin窗口标题栏为非活动颜色
	if (keyWin == sheetWin) {
		cursorC = -1;	// sheetWin光标隐藏
		boxFill8(sheetWin->buf, sheetWin->width, COL8_FFFFFF, cursorX, 28, cursorX + 7, 43);	// 覆盖先前光标
	} else {
		if ((keyWin->status & 0x20) != 0) {	// 目标窗口有光标
			QueuePush(&keyWin->process->queue, 3);	// 向要停止接收键盘输入的窗口进程缓冲区发送光标隐藏信息
		}
	}
	return cursorC;	// 返回应为sheetWin设置的光标颜色
}

/*******************************************************
*
* Function name: keyWinOn
* Description: 使指定窗口开始接收键盘数据
* Parameter:
*	@keyWin		指定的窗口窗口指针							struct SHEET *
*	@sheetWin	主进程ProcessA输入框窗口					struct SHEET *
*	@cursorC	当前主进程ProcessA输入框窗口光标颜色状态	int
* Return:
*	返回应为主进程processA中的输入框设置的光标颜色状态
*
**********************************************************/
int keyWinOn(struct SHEET *keyWin, struct SHEET *sheetWin, int cursorC) {
	changeWinTitle(keyWin, 1);	// 改变keyWin窗口标题栏为活动颜色
	if (keyWin == sheetWin) {	// 指定窗口为主进程ProcessA输入框窗口
		cursorC = COL8_000000;	// sheetWin显示光标
	} else {
		if ((keyWin->status & 0x20) != 0) {	// 目标窗口有光标
			QueuePush(&keyWin->process->queue, 2);	// 向要接收键盘输入的窗口进程缓冲区发送光标显示信息
		}
	}
	return cursorC;
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
	int i, x, y, mx, my, bufval, cursorX, cursorC, mmx = -1, mmy = -1; // 鼠标x轴位置 鼠标y轴位置 光标x轴位置 光标颜色 移动模式x坐标 移动模式y坐标
	int keyShift = 0, keyLeds, keyCmdWait = -1;	// shift按下标识 键盘对应按键灯状态
	struct MouseDec mdec;	// 保存鼠标信息
	unsigned int memtotal;
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;	// 内存段表指针
	struct SHTCTL *shtctl;	// 图层控制块指针
	struct SHEET *sheetBack, *sheetMouse, *sheetWin, *sheetCons, *sheet = 0, *keyWin;	// 背景图层 鼠标图层 窗口图层 控制台图层 遍历用图层指针 当前处于输入模式的窗口指针
	unsigned char *bufBack, bufMouse[256], *bufWin, *bufCons;	// 背景图像缓冲区 鼠标图像缓冲区 窗口图像缓冲区 控制台图像缓冲区
	struct QUEUE queue, keyCmd;	// 总缓冲区 存储欲向键盘控制电路发送的数据的缓冲区
	struct TIMER *timer;	// 四个定时器指针
	struct PCB *processA, *processConsole;
	struct CONSOLE *console;
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
	
	*((int *) 0x0fe4) = (int) shtctl;
	
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
	
	keyWin = sheetWin;
	sheetCons->process = processConsole;
	sheetCons->status |= 0x20;	// 有光标	（status 0x10位标识窗口是否由应用程序生成 0x20位判断是否需要光标）
	
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
			if (keyWin->status == 0) {	// 输入窗口被关闭
				keyWin = shtctl->sheetsAcs[shtctl->top - 1];	// 输入窗口设为当前最顶层窗口
				cursorC = keyWinOn(keyWin, sheetWin, cursorC);
			}
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
					if (keyWin == sheetWin) {	// processA 窗口
						if (cursorX < 128) {
							s[1] = 0;
							putFont8AscSheet(sheetWin, cursorX, 28, COL8_000000, COL8_FFFFFF, s, 1);	// 将字符打印至窗口层
							cursorX += 8;	// 光标后移一个字符
						}
					} else {
						QueuePush(&keyWin->process->queue, s[0] + 256);
					}
				}
				if (bufval == 256 + 0x0e) {	// 按下退格键且光标不在输入框起始位置
					if (keyWin == sheetWin) {
						if (cursorX > 8) {
							putFont8AscSheet(sheetWin, cursorX, 28, COL8_000000, COL8_FFFFFF, " ", 1);	// 用空格消去当前光标
							cursorX -= 8;	// 光标前移一个字符
						}
					} else {
						QueuePush(&keyWin->process->queue, 8 + 256);
					}
					
				}
				if (bufval == 256 + 0x0f) {	// 按下tab键
					cursorC = keyWinOff(keyWin, sheetWin, cursorC, cursorX);
					i = keyWin->index - 1;	// 找到下一层图层索引号
					
					if (i == 0) {	// 如果下一层为背景层
						i = shtctl->top - 1;	// 将下一次索引置为鼠标下最高层
					}
					keyWin = shtctl->sheetsAcs[i];	// 获取当前接收键盘数据的图层
					cursorC = keyWinOn(keyWin, sheetWin, cursorC);
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
				if (bufval == 256 + 0x3b && keyShift != 0 && processConsole->tss.ss0 != 0) {	// shift + F1
					console = (struct CONSOLE *) *((int *) 0x0fec);
					consolePutstr0(console, "\nBreak(key) :\n");
					io_cli();	// 关中断 不能在改变寄存器值的时候切换其他进程
					processConsole->tss.eax = (int) &(processConsole->tss.esp0);
					processConsole->tss.eip = (int) asm_endApp;
					io_sti();	// 开中断
				}
				if (bufval == 256 + 0xfa) {	// 键盘控制电路返回成功数据
					keyCmdWait = -1; // 等待进行下一次发送
				}
				if (bufval == 256 + 0xfe) {	// 键盘控制电路返回失败数据
					waitKeyboardControllerReady();	// 等待键盘控制电路就绪
					io_out8(PORT_KEYDAT, keyCmdWait);	// 重新向键盘控制电路发送8位数据
				}
				if (bufval == 256 + 0x1c) {	// 回车
					if (keyWin != sheetWin) {	// 发送至控制台窗口
						QueuePush(&keyWin->process->queue, 10 + 256);
					}
				}
				if (bufval == 256 + 0x57 && shtctl->top > 2) {	// 按下F11 且最上层图层索引大于2 除去鼠标和背景还存在至少2个图层
					sheetUpdown(shtctl->sheetsAcs[1], shtctl->top - 1);	// 将除去背景层之外处于最下层的图层放在鼠标下最高层
				}
				// 重新显示光标
				if (cursorC >= 0) {
					boxFill8(sheetWin->buf, sheetWin->width, cursorC, cursorX, 28, cursorX + 7, 43);
				}
				sheetRefresh(sheetWin, cursorX, 28, cursorX + 8, 44);
			} else if (512 <= bufval && bufval <= 767) {	// 鼠标中断数据
				if (mouseDecode(&mdec, bufval - 512) != 0) {	// 完成一波三个字节数据的接收或者出现未知差错
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					if ((mdec.btn & 0x01) != 0) {	// 按下左键
						s[1] = 'L';
						// sheetSlide(sheetWin, mx - 80, my - 8);
						if (mmx < 0) {	// mmx = -1 mmy = -1 时不处于移动模式
							for (i = shtctl->top - 1; i > 0; i--) {	// 由鼠标下第一层开始，自顶向下遍历图层
								sheet = shtctl->sheetsAcs[i];
								x = mx - sheet->locationX;
								y = my - sheet->locationY;
								if (0 <= x && x < sheet->width && 0 <= y && y < sheet->height) {	// 鼠标点中的区域属于该图层
									if (sheet->buf[y * sheet->width + x] != sheet->colInvNum) {	// 当前鼠标点中的区域不为该图层透明色
										sheetUpdown(sheet, shtctl->top - 1);	// 将鼠标点中的图层移植鼠标下最高层
										if (sheet != keyWin) {
											cursorC = keyWinOff(keyWin, sheetWin, cursorC, cursorX);
											keyWin = sheet;
											cursorC = keyWinOn(keyWin, sheetWin, cursorC);
										}
										if (3 <= x && x < sheet->width - 3 && 3 <= y && y < 21) {	// 当前鼠标点中的为窗口标题栏
											mmx = mx;	// 移动模式记录鼠标当前坐标
											mmy = my;
										}
										if (sheet->width - 21 <= x && x < sheet->width - 5 && 5 <= y && y < 19) {	// 当前鼠标点中的为窗口关闭按钮
											if ((sheet->status & 0x10) != 0) {	// 该窗口隶属某一应用程序
												console = (struct CONSOLE *) *((int *) 0x0fec);
												consolePutstr0(console, "\n Break(mouse) :\n");
												io_cli();	// 强制结束处理时关中断
												processConsole->tss.eax = (int) &(processConsole->tss.esp0);
												processConsole->tss.eip = (int) asm_endApp;
												io_sti();	// 开中断	
											}
										}
										break;
									}
								}
							}
						} else {
							// 计算鼠标移动的距离
							x = mx - mmx;
							y = my - mmy;
							sheetSlide(sheet, sheet->locationX + x, sheet->locationY + y);	// 移动图层
							// 更新移动后的坐标
							mmx = mx;
							mmy = my;
						}
					} else {	// 没有按下左键
						mmx = mmy = -1;	// 推出移动模式
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
