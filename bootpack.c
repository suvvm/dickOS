/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 0.6.2
* @Date: 2020-02-11
* @Description: ����������Ҫʹ�õĹ��ܺ���
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
* Description: ʹ��ǰ���ڻ�ȡ�������ݵĴ���ֹͣ��ȡ��������
* Parameter:
*	@keyWin		��ǰ���ڽ��ռ������ݵĴ���ָ��				struct SHEET *
*	@sheetWin	������ProcessA����򴰿�					struct SHEET *
*	@cursorC	��ǰ������ProcessA����򴰿ڹ����ɫ״̬	int
*	@cursorX	��ǰ������ProcessA����򴰿ڹ��λ��		int
* Return:
*	����ӦΪ������processA�е���������õĹ����ɫ״̬
*
**********************************************************/
int keyWinOff(struct SHEET *keyWin, struct SHEET *sheetWin, int cursorC, int cursorX) {
	changeWinTitle(keyWin, 0);	// �ı�keyWin���ڱ�����Ϊ�ǻ��ɫ
	if (keyWin == sheetWin) {
		cursorC = -1;	// sheetWin�������
		boxFill8(sheetWin->buf, sheetWin->width, COL8_FFFFFF, cursorX, 28, cursorX + 7, 43);	// ������ǰ���
	} else {
		if ((keyWin->status & 0x20) != 0) {	// Ŀ�괰���й��
			QueuePush(&keyWin->process->queue, 3);	// ��Ҫֹͣ���ռ�������Ĵ��ڽ��̻��������͹��������Ϣ
		}
	}
	return cursorC;	// ����ӦΪsheetWin���õĹ����ɫ
}

/*******************************************************
*
* Function name: keyWinOn
* Description: ʹָ�����ڿ�ʼ���ռ�������
* Parameter:
*	@keyWin		ָ���Ĵ��ڴ���ָ��							struct SHEET *
*	@sheetWin	������ProcessA����򴰿�					struct SHEET *
*	@cursorC	��ǰ������ProcessA����򴰿ڹ����ɫ״̬	int
* Return:
*	����ӦΪ������processA�е���������õĹ����ɫ״̬
*
**********************************************************/
int keyWinOn(struct SHEET *keyWin, struct SHEET *sheetWin, int cursorC) {
	changeWinTitle(keyWin, 1);	// �ı�keyWin���ڱ�����Ϊ���ɫ
	if (keyWin == sheetWin) {	// ָ������Ϊ������ProcessA����򴰿�
		cursorC = COL8_000000;	// sheetWin��ʾ���
	} else {
		if ((keyWin->status & 0x20) != 0) {	// Ŀ�괰���й��
			QueuePush(&keyWin->process->queue, 2);	// ��Ҫ���ռ�������Ĵ��ڽ��̻��������͹����ʾ��Ϣ
		}
	}
	return cursorC;
}

/*******************************************************
*
* Function name:Main
* Description: ������
*
**********************************************************/
void Main(){
	struct BOOTINFO *binfo;
	char s[40];
	int	buf[128], keyCmdBuf[32];	// s����Ҫ����ı�����Ϣ bufΪ�ܻ�����
	int i, x, y, mx, my, bufval, cursorX, cursorC, mmx = -1, mmy = -1; // ���x��λ�� ���y��λ�� ���x��λ�� �����ɫ �ƶ�ģʽx���� �ƶ�ģʽy����
	int keyShift = 0, keyLeds, keyCmdWait = -1;	// shift���±�ʶ ���̶�Ӧ������״̬
	struct MouseDec mdec;	// ���������Ϣ
	unsigned int memtotal;
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;	// �ڴ�α�ָ��
	struct SHTCTL *shtctl;	// ͼ����ƿ�ָ��
	struct SHEET *sheetBack, *sheetMouse, *sheetWin, *sheetCons, *sheet = 0, *keyWin;	// ����ͼ�� ���ͼ�� ����ͼ�� ����̨ͼ�� ������ͼ��ָ�� ��ǰ��������ģʽ�Ĵ���ָ��
	unsigned char *bufBack, bufMouse[256], *bufWin, *bufCons;	// ����ͼ�񻺳��� ���ͼ�񻺳��� ����ͼ�񻺳��� ����̨ͼ�񻺳���
	struct QUEUE queue, keyCmd;	// �ܻ����� �洢������̿��Ƶ�·���͵����ݵĻ�����
	struct TIMER *timer;	// �ĸ���ʱ��ָ��
	struct PCB *processA, *processConsole;
	struct CONSOLE *console;
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;	// ��ȡ������Ϣ
	keyLeds = (binfo->leds >> 4) & 7;	// ��ȡ���̸�������״̬ binfo->leds��4~6λ
	initGdtit();	// ��ʼ��GDT IDT
	init_pic();	// ��ʼ���ɱ���жϿ�����
	io_sti();	// ���cpu�жϽ�ֹ
		
	QueueInit(&queue, 128, buf, 0);	// ��ʼ������������
	
	initPit();	// ��ʼ����ʱ��
	initKeyboard(&queue, 256);	// ��ʼ�����̿��Ƶ�·
	enableMouse(&queue, 512, &mdec);	// �������
	
	io_out8(PIC0_IMR, 0xf8); // ��PIC IRQ0(��ʱ��) IRQ1�����̣���IRQ2����PIC����������(11111000)
	io_out8(PIC1_IMR, 0xef); // ��PIC IRQ12����꣩��������(11101111)
	QueueInit(&keyCmd, 32, keyCmdBuf, 0);

	timer = timerAlloc();	// ��ȡһ����ʹ�õĶ�ʱ��
	timerInit(timer, &queue, 1);	// ��ʼ����ʱ��1
	timerSetTime(timer, 50);
	
	memtotal = memtest(0x00400000, 0xbfffffff);	// ��ȡ�ڴ��ܺ�
	memsegInit(memsegtable);
	memsegFree(memsegtable, 0x00001000, 0x0009e000);
	memsegFree(memsegtable, 0x00400000, memtotal - 0x00400000);
	
	init_palette();	// ��ʼ��16ɫ��ɫ��
	
	shtctl = shtctlInit(memsegtable, binfo->vram, binfo->scrnx, binfo->scrny);
	
	processA = processInit(memsegtable);	// ����̳�ʼ��
	queue.process = processA;
	processRun(processA, 1, 0);	// ����A���ڵ�1��
	
	*((int *) 0x0fe4) = (int) shtctl;
	
	sheetBack = sheetAlloc(shtctl);
	bufBack = (unsigned char *) memsegAlloc4K(memsegtable, binfo->scrnx * binfo->scrny);	// ��4KBΪ��λΪ���������ڴ�
	sheetSetbuf(sheetBack, bufBack, binfo->scrnx, binfo->scrny, -1);	// ���ñ���ͼ�㻺��������������Ҫ͸��ɫ��Ϊ-1
	init_GUI(bufBack, binfo->scrnx, binfo->scrny);	// ��ʼ��GUI��bufBack
	
	sheetCons = sheetAlloc(shtctl);	// Ϊ����̨����ͼ��
	bufCons = (unsigned char *) memsegAlloc4K(memsegtable, 256 * 165);
	sheetSetbuf(sheetCons, bufCons, 256, 165, -1);	// ���ÿ���̨ͼ�㻺����������̨����Ҫ͸��ɫ
	makeWindow(bufCons, 256, 165, "console", 0);	// ��������̨����
	makeTextBox(sheetCons, 8, 28, 240, 128, COL8_000000);	// ��������̨��ɫ�����
	
	processConsole = processAlloc();	// �������̨����
	processConsole->tss.esp = memsegAlloc4K(memsegtable, 64 * 1024) + 64 * 1024 - 12;	// Ϊ����̨����ջ����64KB�ڴ沢����ջ�׵�ַ��B��ESP Ϊ���·���ֵʱ�����ڴ淶Χ �����ȥ8	
	*((int *) (processConsole->tss.esp + 4)) = (int) sheetCons;	// ��sheetCons��ַ�����ڴ��ַ esp + 4 c���Ժ���ָ���Ĳ�����ESP+4��λ��
	*((int *) (processConsole->tss.esp + 8)) = memtotal;
	processConsole->tss.eip = (int) &consoleMain;	// ����̨���̵���һ��ָ��ִ��consoleMain
	
	processConsole->tss.es = 1 * 8;
	processConsole->tss.cs = 2 * 8;
	processConsole->tss.ss = 1 * 8;
	processConsole->tss.ds = 1 * 8;
	processConsole->tss.fs = 1 * 8;
	processConsole->tss.gs = 1 * 8;
	
	processRun(processConsole, 2, 2);	
	// ����̨���� level2 2 �����������
	
	sheetWin = sheetAlloc(shtctl);
	bufWin = (unsigned char *) memsegAlloc4K(memsegtable, 160 * 52);
	sheetSetbuf(sheetWin, bufWin, 144, 52, -1);	// ���ô���ͼ�㻺���� ��͸��ɫ
	makeWindow(bufWin, 144, 52, "processA", 1);	// ��ʾ����
	makeTextBox(sheetWin, 8, 28, 128, 16, COL8_FFFFFF);	// ��ʾ�ı�������
	
	cursorX = 8;	// ���ù����ʼλ��
	cursorC = COL8_FFFFFF;	// ���ù����ɫ
	sheetMouse = sheetAlloc(shtctl);
	sheetSetbuf(sheetMouse, bufMouse, 16, 16, COL8_008484);	// �������ͼ�㻺������͸��ɫ
	initMouseCursor8(bufMouse, COL8_008484);	// ��ʼ�������bufMouse
	// ��ʼ�������Ϊ��Ļ����
	mx = (binfo->scrnx - 16) / 2;	// ���x��λ��
	my = (binfo->scrny - 28 - 16) / 2;	// ���y��λ��
	
	sheetSlide(sheetBack, 0, 0);	// ����ͼ����ʼ����(0,0)
	sheetSlide(sheetCons, 168, 64);	// ����̨���̴�����ʼ����(168, 56)
	sheetSlide(sheetWin, 8, 64);	// ����A ������ʼ����(8, 56);
	sheetSlide(sheetMouse, mx, my);	// �����ͼ�㻬������Ӧλ��
	
	sheetUpdown(sheetBack, 0);	// ������ͼ����������0
	sheetUpdown(sheetCons, 1);	// ������̨���̴���ͼ����������1
	sheetUpdown(sheetWin, 2);	// ������A����ͼ��������������4
	sheetUpdown(sheetMouse, 3);	// �����ͼ����������5
	
	putFont8AscSheet(sheetBack, 0, 32, COL8_FFFFFF, COL8_008484,  "Welcome to DickOS", 17);	// ��DickOSд�뱳����
	sprintf(s, "(%3d, %3d)", mx, my);	// �����λ�ô���s
	putFont8AscSheet(sheetBack, 0, 0, COL8_FFFFFF, COL8_008484,  s, 10);	// ��sд�뱳����
	
	keyWin = sheetWin;
	sheetCons->process = processConsole;
	sheetCons->status |= 0x20;	// �й��	��status 0x10λ��ʶ�����Ƿ���Ӧ�ó������� 0x20λ�ж��Ƿ���Ҫ��꣩
	
	// Ϊ�˱���ͼ��̵�ǰ״̬��ͻ����һ��ʼ�Ƚ�������ָʾ��״̬
	QueuePush(&keyCmd, KEYCMD_LED);	// 0xed
	QueuePush(&keyCmd, keyLeds);
	for(;;){
		if (QueueSize(&keyCmd) > 0 && keyCmdWait < 0) {	// �������Ҫ����̿��������͵����ݾͷ�����
			keyCmdWait = QueuePop(&keyCmd);
			waitKeyboardControllerReady();
			io_out8(PORT_KEYDAT, keyCmdWait);	// ����̿��Ƶ�·����
		}
		io_cli();	// ���ж�
		if(QueueSize(&queue) == 0) {	// ֻ�л�����û������ʱ���ܿ����ж�
			processSleep(processA);	
			// ����A���� ��A����������Զ�������������B
			io_sti();	// ���ж�
		} else {
			bufval = QueuePop(&queue);	// ȡ�����������ж�������
			io_sti();	// ���ж�
			if (keyWin->status == 0) {	// ���봰�ڱ��ر�
				keyWin = shtctl->sheetsAcs[shtctl->top - 1];	// ���봰����Ϊ��ǰ��㴰��
				cursorC = keyWinOn(keyWin, sheetWin, cursorC);
			}
			if (256 <= bufval && bufval <= 511) {	// �����ж�����
				sprintf(s, "%02X", bufval - 256);
				putFont8AscSheet(sheetBack, 0, 16, COL8_FFFFFF, COL8_008484, s, 2);	// �������ж���Ϣ��ӡ��������
				if (bufval < 256 + 0x80) {	// ���¼���
					if (keyShift == 0) {	// ��δ����shift
						s[0] = keyboardTable0[bufval - 256];
					} else {	// shift���ڰ���
						s[0] = keyboardTable1[bufval - 256];
					}
				} else {
					s[0] = 0;
				}
				
				if ('A' <= s[0] && s[0] <= 'Z') {	// s[0]ΪӢ����ĸ
					if (((keyLeds & 4) == 0 && keyShift == 0) || ((keyLeds & 4) != 0 && keyShift != 0)) {	// ��Ҫת��ΪСд
						s[0] += 0x20;
					}
				}
				
				if (s[0] != 0) {	// ��ͨ�ַ�
					if (keyWin == sheetWin) {	// processA ����
						if (cursorX < 128) {
							s[1] = 0;
							putFont8AscSheet(sheetWin, cursorX, 28, COL8_000000, COL8_FFFFFF, s, 1);	// ���ַ���ӡ�����ڲ�
							cursorX += 8;	// ������һ���ַ�
						}
					} else {
						QueuePush(&keyWin->process->queue, s[0] + 256);
					}
				}
				if (bufval == 256 + 0x0e) {	// �����˸���ҹ�겻���������ʼλ��
					if (keyWin == sheetWin) {
						if (cursorX > 8) {
							putFont8AscSheet(sheetWin, cursorX, 28, COL8_000000, COL8_FFFFFF, " ", 1);	// �ÿո���ȥ��ǰ���
							cursorX -= 8;	// ���ǰ��һ���ַ�
						}
					} else {
						QueuePush(&keyWin->process->queue, 8 + 256);
					}
					
				}
				if (bufval == 256 + 0x0f) {	// ����tab��
					cursorC = keyWinOff(keyWin, sheetWin, cursorC, cursorX);
					i = keyWin->index - 1;	// �ҵ���һ��ͼ��������
					
					if (i == 0) {	// �����һ��Ϊ������
						i = shtctl->top - 1;	// ����һ��������Ϊ�������߲�
					}
					keyWin = shtctl->sheetsAcs[i];	// ��ȡ��ǰ���ռ������ݵ�ͼ��
					cursorC = keyWinOn(keyWin, sheetWin, cursorC);
				}
				if (bufval == 256 + 0x2a) {	// ��shift����
					keyShift |= 1;
				}
				if (bufval == 256 + 0x36) {	// ��shift����
					keyShift |= 2;
				}
				if (bufval == 256 + 0xaa) {	// ��shifţ��
					keyShift &= ~1;
				}
				if (bufval == 256 + 0xb6) {	// ��shifţ��
					keyShift &= ~2;
				}
				if (bufval == 256 + 0x3a) {	// CapsLock
					keyLeds ^= 4;	// �ı�keyLeds��Ӧλ
					QueuePush(&keyCmd, KEYCMD_LED);
					QueuePush(&keyCmd, keyLeds);
					// ��0xed keyLeds���뻺�����ȴ�����̿��Ƶ�·����
				}
				if (bufval == 256 + 0x45) {	// NumLock
					keyLeds ^= 2;	// �ı�keyLeds��Ӧλ
					QueuePush(&keyCmd, KEYCMD_LED);
					QueuePush(&keyCmd, keyLeds);
					// ��0xed keyLeds���뻺�����ȴ�����̿��Ƶ�·����
				}
				if (bufval == 256 + 0x46) {	// ScrollLock
					keyLeds ^= 1;	// �ı�keyLeds��Ӧλ
					QueuePush(&keyCmd, KEYCMD_LED);
					QueuePush(&keyCmd, keyLeds);
					// ��0xed keyLeds���뻺�����ȴ�����̿��Ƶ�·����
				}
				if (bufval == 256 + 0x3b && keyShift != 0 && processConsole->tss.ss0 != 0) {	// shift + F1
					console = (struct CONSOLE *) *((int *) 0x0fec);
					consolePutstr0(console, "\nBreak(key) :\n");
					io_cli();	// ���ж� �����ڸı�Ĵ���ֵ��ʱ���л���������
					processConsole->tss.eax = (int) &(processConsole->tss.esp0);
					processConsole->tss.eip = (int) asm_endApp;
					io_sti();	// ���ж�
				}
				if (bufval == 256 + 0xfa) {	// ���̿��Ƶ�·���سɹ�����
					keyCmdWait = -1; // �ȴ�������һ�η���
				}
				if (bufval == 256 + 0xfe) {	// ���̿��Ƶ�·����ʧ������
					waitKeyboardControllerReady();	// �ȴ����̿��Ƶ�·����
					io_out8(PORT_KEYDAT, keyCmdWait);	// ��������̿��Ƶ�·����8λ����
				}
				if (bufval == 256 + 0x1c) {	// �س�
					if (keyWin != sheetWin) {	// ����������̨����
						QueuePush(&keyWin->process->queue, 10 + 256);
					}
				}
				if (bufval == 256 + 0x57 && shtctl->top > 2) {	// ����F11 �����ϲ�ͼ����������2 ��ȥ���ͱ�������������2��ͼ��
					sheetUpdown(shtctl->sheetsAcs[1], shtctl->top - 1);	// ����ȥ������֮�⴦�����²��ͼ������������߲�
				}
				// ������ʾ���
				if (cursorC >= 0) {
					boxFill8(sheetWin->buf, sheetWin->width, cursorC, cursorX, 28, cursorX + 7, 43);
				}
				sheetRefresh(sheetWin, cursorX, 28, cursorX + 8, 44);
			} else if (512 <= bufval && bufval <= 767) {	// ����ж�����
				if (mouseDecode(&mdec, bufval - 512) != 0) {	// ���һ�������ֽ����ݵĽ��ջ��߳���δ֪���
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					if ((mdec.btn & 0x01) != 0) {	// �������
						s[1] = 'L';
						// sheetSlide(sheetWin, mx - 80, my - 8);
						if (mmx < 0) {	// mmx = -1 mmy = -1 ʱ�������ƶ�ģʽ
							for (i = shtctl->top - 1; i > 0; i--) {	// ������µ�һ�㿪ʼ���Զ����±���ͼ��
								sheet = shtctl->sheetsAcs[i];
								x = mx - sheet->locationX;
								y = my - sheet->locationY;
								if (0 <= x && x < sheet->width && 0 <= y && y < sheet->height) {	// �����е��������ڸ�ͼ��
									if (sheet->buf[y * sheet->width + x] != sheet->colInvNum) {	// ��ǰ�����е�����Ϊ��ͼ��͸��ɫ
										sheetUpdown(sheet, shtctl->top - 1);	// �������е�ͼ����ֲ�������߲�
										if (sheet != keyWin) {
											cursorC = keyWinOff(keyWin, sheetWin, cursorC, cursorX);
											keyWin = sheet;
											cursorC = keyWinOn(keyWin, sheetWin, cursorC);
										}
										if (3 <= x && x < sheet->width - 3 && 3 <= y && y < 21) {	// ��ǰ�����е�Ϊ���ڱ�����
											mmx = mx;	// �ƶ�ģʽ��¼��굱ǰ����
											mmy = my;
										}
										if (sheet->width - 21 <= x && x < sheet->width - 5 && 5 <= y && y < 19) {	// ��ǰ�����е�Ϊ���ڹرհ�ť
											if ((sheet->status & 0x10) != 0) {	// �ô�������ĳһӦ�ó���
												console = (struct CONSOLE *) *((int *) 0x0fec);
												consolePutstr0(console, "\n Break(mouse) :\n");
												io_cli();	// ǿ�ƽ�������ʱ���ж�
												processConsole->tss.eax = (int) &(processConsole->tss.esp0);
												processConsole->tss.eip = (int) asm_endApp;
												io_sti();	// ���ж�	
											}
										}
										break;
									}
								}
							}
						} else {
							// ��������ƶ��ľ���
							x = mx - mmx;
							y = my - mmy;
							sheetSlide(sheet, sheet->locationX + x, sheet->locationY + y);	// �ƶ�ͼ��
							// �����ƶ��������
							mmx = mx;
							mmy = my;
						}
					} else {	// û�а������
						mmx = mmy = -1;	// �Ƴ��ƶ�ģʽ
					}
					if ((mdec.btn & 0x02) != 0) {	// �����Ҽ�
						s[3] = 'R';
					}
					if ((mdec.btn & 0x04) != 0) {	// �м����
						s[2] = 'C';
					}
					putFont8AscSheet(sheetBack, 32, 16, COL8_FFFFFF, COL8_008484, s, 15);	// �ڱ������ӡ��갴����Ϣ
					
					// ����������ƫ����
					mx += mdec.x;
					my += mdec.y;
					
					// ���߽紦��
					if (mx < 0)
						mx = 0;
					if (my < 0)
						my = 0;
					if (mx > binfo->scrnx - 1)
						mx = binfo->scrnx - 1;
					if (my > binfo->scrny - 1)
						my = binfo->scrny - 1;
					
					sprintf(s, "(%3d, %3d)", mx, my);
					putFont8AscSheet(sheetBack, 0, 0, COL8_FFFFFF, COL8_008484, s, 10);	// �ڱ������ӡ���������Ϣ
					sheetSlide(sheetMouse, mx, my);
				} 			
			} else if (bufval <= 1) {
				if (bufval == 1) {
					timerInit(timer, &queue, 0);	// ��ʱ��Ϣ��0
					if (cursorC >= 0) {
						cursorC = COL8_000000;
					}
				} else {
					timerInit(timer, &queue, 1);	// ��ʱ��Ϣ��1
					if (cursorC >= 0) {
						cursorC = COL8_FFFFFF;
					}
				}
				timerSetTime(timer, 50);	// ���ó�ʱʱ��0.5��
				if (cursorC >= 0) {
					boxFill8(sheetWin->buf, sheetWin->width, cursorC, cursorX, 28, cursorX + 7, 43);
					sheetRefresh(sheetWin, cursorX, 28, cursorX + 8, 44);
				}
			}
		}
	}
}
