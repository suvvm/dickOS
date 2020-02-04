/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 0.4.1
* @Date: 2020-02-04
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

/********************************************************
*
* Function name: consoleMain
* Description: ����̨����������
*
**********************************************************/
void consoleMain(struct SHEET *sheet) {
	struct QUEUE queue;	// ����������
	struct TIMER * timer;	// ��ʱ��
	struct PCB *process = processNow();	// ȡ�õ�ǰռ�д�����Ľ���
	
	int bufval, buf[128], cursorX = 8, cursorC = COL8_000000;	// �������ַ�ֵ ������ ���x��λ�� �����ɫ
	
	QueueInit(&queue, 128, buf, process);	// ��ʼ������������
	timer = timerAlloc();
	timerInit(timer, &queue, 1);
	timerSetTime(timer, 50);	// 0.5�볬ʱ
	
	for(;;) {
		io_cli();
		if (QueueSize(&queue) == 0) {
			processSleep(process);	// ����ʱ��������
			io_sti();	// ���ж�
		} else {
			bufval = QueuePop(&queue);
			io_sti();	// ���ж�
			if (bufval <= 1) {	// ��궨ʱ����ʱ
				if (bufval != 0) {
					timerInit(timer, &queue, 0);
					cursorC = COL8_FFFFFF;	// ����ɫ
				} else {
					timerInit(timer, &queue, 1);
					cursorC = COL8_000000;	// ����ɫ
				}
				timerSetTime(timer, 50);
				boxFill8(sheet->buf, sheet->width, cursorC, cursorX, 28, cursorX + 7, 43);	// ���»��ƹ��
				sheetRefresh(sheet, cursorX, 28, cursorX + 8, 44);
				
			}
		}
	}
}

/*******************************************************
*
* Function name: makeTextBox
* Description: �����ı������
* Parameter:
*	@sheet	ͼ��ָ��	struct SHEET *
*	@startX	x����ʼλ��	int
*	@startY	y����ʼλ��	int
*	@width	���		int
*	@height	�߶�		int
*	@c		�ı�����ɫ	int
*
**********************************************************/
void makeTextBox(struct SHEET *sheet, int startX, int startY, int width, int height, int c) {
	int endX = startX + width, endY = startY + height;
	boxFill8(sheet->buf, sheet->width, COL8_848484, startX - 2, startY - 3, endX + 1, startY - 3);	// ����ɫ (startX-2,startY-3)~(endX+1,startY-3) һ���ϲ��߿����
	boxFill8(sheet->buf, sheet->width, COL8_848484, startX - 3, startY - 3, startX - 3, endY + 1);	// ����ɫ (startX-3,startY-3)~(startX-3,endY+1) һ�����߿�����
	boxFill8(sheet->buf, sheet->width, COL8_FFFFFF, startX - 3, endY + 2, endX + 1, endY + 2);		// ��ɫ (startX-3,endY+2)~(endX+1,endY+2) һ���ײ��߿����
	boxFill8(sheet->buf, sheet->width, COL8_FFFFFF, endX + 2, startY - 3, endX + 2, endY + 2);		// ��ɫ (endX+2,startY-2)~(endX+2,endY+2) һ���Ҳ�߿�����
	boxFill8(sheet->buf, sheet->width, COL8_000000, startX - 1, startY - 2, endX, startY - 2);		// ��ɫ	(startX-1,startY-2)~(endX,startY-2) һ�������߿����
	boxFill8(sheet->buf, sheet->width, COL8_000000, startX - 2, startY - 2, startX - 2, endY);		// ��ɫ (startX-2,startY-2)~(startX-2,endY) һ�����߿�����
	boxFill8(sheet->buf, sheet->width, COL8_C6C6C6, startX - 2, endY + 1, endX, endY + 1);			// ����ɫ (startX-2,endY+1,endX,endY+1) һ���ײ��߿����
	boxFill8(sheet->buf, sheet->width, COL8_C6C6C6, endX + 1, startY - 2, endX + 1, endY + 1);		// ����ɫ (endX+1,startY-2)~(endX+1,endY+1) һ���Ҳ�߿�����
	
	boxFill8(sheet->buf, sheet->width, c, startX - 1, startY - 1, endX, endY);	// �ı�������
}

/*******************************************************
*
* Function name: makeWindow
* Description: ��������
* Parameter:
*	@buf	����ͼ�񻺳���	unsigned char *
*	@width	���ڿ��		int
*	@height	���ڸ߶�		int
*	@title	���ڱ���		char *
*	@act	���ʶ		char
*
**********************************************************/
void makeWindow(unsigned char *buf, int width, int height, char *title, char act) {
	static char closeBtn[14][16] = {	// �رհ�ť
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
	char c, tc, tbc;	// closeBtn��Ӧλ���ַ� title������ɫ ���ڱ�������ɫ
	if (act != 0) {	// ����Ϊ�����
		tc = COL8_FFFFFF;	// ��ɫ
		tbc = COL8_000084;	// ����ɫ
	} else {	// ���ڲ�Ϊ�����
		tc = COL8_C6C6C6;	// ����ɫ
		tbc = COL8_848484;	// ����ɫ
	}
	// ���ƴ��ڣ����궼Ϊ������꣬��ʼ(0,0)��
	boxFill8(buf, width, COL8_C6C6C6, 0, 0, width - 1, 0);	// ����ɫ (0,0)~(width-1,0)��������
	boxFill8(buf, width, COL8_FFFFFF, 1, 1, width - 2, 1);	// ��ɫ (1,1)~(width-2,1)��������
	boxFill8(buf, width, COL8_C6C6C6, 0, 0, 0, height - 1);	// ����ɫ (0,0)~(0,height-1)�������
	boxFill8(buf, width, COL8_FFFFFF, 1, 1, 1, height - 2);	// ��ɫ (1,1)~(1,height-2)�������
	boxFill8(buf, width, COL8_848484, width - 2, 1, width - 2, height - 2);	//����ɫ (width - 2,1)~(width - 2, height - 2)һ���Ҳ�����
	boxFill8(buf, width, COL8_000000, width - 1, 0, width - 1, height - 1);	// ��ɫ (width - 1,0)~(width - 1, height - 1)һ���Ҳ�����
	boxFill8(buf, width, COL8_C6C6C6, 2, 2, width - 3, height - 3);	// ����ɫ (2,2)~(width - 3,height - 3) ���ľ���
	boxFill8(buf, width, tbc, 3, 3, width - 4, 20);	// tbc(3,3)~(width - 4, 20)��������
	boxFill8(buf, width, COL8_848484, 1, height - 2, width - 2, height - 2);	// ����ɫ(1,height-2)~(width-2,height-2)�ײ�����
	boxFill8(buf, width, COL8_000000, 0, height - 1, width - 1, height - 1);	// ��ɫ(0,height-1)~(width-1,height-1)�ײ�����
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
* Description: ������
*
**********************************************************/
void Main(){
	struct BOOTINFO *binfo;
	char s[40];
	int	buf[128];	// s����Ҫ����ı�����Ϣ bufΪ�ܻ�����
	int mx, my, bufval, cursorX, cursorC; //���x��λ�� ���y��λ�� ���x��λ�� �����ɫ
	struct MouseDec mdec;	// ���������Ϣ
	unsigned int memtotal;
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;	// �ڴ�α�ָ��
	struct SHTCTL *shtctl;	// ͼ����ƿ�ָ��
	struct SHEET *sheetBack, *sheetMouse, *sheetWin, *sheetCons;	// ����ͼ�� ���ͼ�� ����ͼ�� ����̨ͼ��
	unsigned char *bufBack, bufMouse[256], *bufWin, *bufCons;	// ����ͼ�񻺳��� ���ͼ�񻺳��� ����ͼ�񻺳��� ����̨ͼ�񻺳���
	struct QUEUE queue;	// �ܻ�����
	struct TIMER *timer;	// �ĸ���ʱ��ָ��
	struct PCB *processA, *processConsole;
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;	// ��ȡ������Ϣ
	
	initGdtit();	// ��ʼ��GDT IDT
	init_pic();	// ��ʼ���ɱ���жϿ�����
	io_sti();	// ���cpu�жϽ�ֹ
		
	QueueInit(&queue, 128, buf, 0);	// ��ʼ������������
	
	initPit();	// ��ʼ����ʱ��
	initKeyboard(&queue, 256);	// ��ʼ�����̿��Ƶ�·
	enableMouse(&queue, 512, &mdec);	// �������
	
	io_out8(PIC0_IMR, 0xf8); // ��PIC IRQ0(��ʱ��) IRQ1�����̣���IRQ2����PIC����������(11111000)
	io_out8(PIC1_IMR, 0xef); // ��PIC IRQ12����꣩��������(11101111)

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
	processConsole->tss.esp = memsegAlloc4K(memsegtable, 64 * 1024) + 64 * 1024 - 8;	// Ϊ����̨����ջ����64KB�ڴ沢����ջ�׵�ַ��B��ESP Ϊ���·���ֵʱ�����ڴ淶Χ �����ȥ8	
	*((int *) (processConsole->tss.esp + 4)) = (int) sheetCons;	// ��sheetCons��ַ�����ڴ��ַ esp + 4 c���Ժ���ָ���Ĳ�����ESP+4��λ��
	
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
	sprintf(s, "memory %dMB free : %dKB", memtest(0x00400000, 0xbfffffff) / (1024 * 1024), memsegTotal(memsegtable) / 1024);	// ���ڴ���Ϣ����s
	putFont8AscSheet(sheetBack, 0, 48, COL8_FFFFFF, COL8_008484,  s, 26);	// ��sд�뱳����
	
	//�����ж������hlt
	
	for(;;){
		io_cli();	// ���ж�
		if(QueueSize(&queue) == 0) {	// ֻ�л�����û������ʱ���ܿ����ж�
			processSleep(processA);	
			// ����A���� ��A����������Զ�������������B
			io_sti();	// ���ж�
		} else {
			bufval = QueuePop(&queue);	// ȡ�����������ж�������
			io_sti();	// ���ж�
			if (256 <= bufval && bufval <= 511) {	// �����ж�����
				sprintf(s, "%02X", bufval - 256);
				putFont8AscSheet(sheetBack, 0, 16, COL8_FFFFFF, COL8_008484, s, 2);	// �������ж���Ϣ��ӡ��������
				if (bufval < 256 + 0x80) {	// ���¼���
					if (keyboardTable[bufval - 256] != 0 && cursorX < 144) {	// ���̶�Ӧֵ���ַ��ҹ��û�е������ĩβ
						s[0] = keyboardTable[bufval - 256];
						s[1] = 0;
						putFont8AscSheet(sheetWin, cursorX, 28, COL8_000000, COL8_FFFFFF, s, 1);	// ���ַ���ӡ�����ڲ�
						cursorX += 8;	// ������һ���ַ�
					}
				}
				if (bufval == 256 + 0x0e && cursorX > 8) {	// �����˸���ҹ�겻���������ʼλ��
					putFont8AscSheet(sheetWin, cursorX, 28, COL8_000000, COL8_FFFFFF, " ", 1);	// �ÿո���ȥ��ǰ���
					cursorX -= 8;	// ���ǰ��һ���ַ�
				}
				// ������ʾ���
				boxFill8(sheetWin->buf, sheetWin->width, cursorC, cursorX, 28, cursorX + 7, 43);
				sheetRefresh(sheetWin, cursorX, 28, cursorX + 8, 44);
			} else if (512 <= bufval && bufval <= 767) {	// ����ж�����
				if (mouseDecode(&mdec, bufval - 512) != 0) {	// ���һ�������ֽ����ݵĽ��ջ��߳���δ֪���
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					if ((mdec.btn & 0x01) != 0) {	// �������
						s[1] = 'L';
						sheetSlide(sheetWin, mx - 80, my - 8);
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
					cursorC = COL8_000000;	
				} else {
					timerInit(timer, &queue, 1);	// ��ʱ��Ϣ��1
					cursorC = COL8_FFFFFF;
				}
				boxFill8(sheetWin->buf, sheetWin->width, cursorC, cursorX, 28, cursorX + 7, 43);
				timerSetTime(timer, 50);	// ���ó�ʱʱ��0.5��
				sheetRefresh(sheetWin, cursorX, 28, cursorX + 8, 44);
			}
		}
	}
}
