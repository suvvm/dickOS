/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 0.2.4
* @Date: 2020-01-30
* @Description: ����������Ҫʹ�õĹ��ܺ���
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
* Description: ��������
* Parameter:
*	@buf	����ͼ�񻺳���	unsigned char *
*	@width	���ڿ��		int
*	@height	���ڸ߶�		int
*	@title	���ڱ���		char *
*
**********************************************************/
void makeWindow(unsigned char *buf, int width, int height, char *title) {
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
	char c;
	// ���ƴ��ڣ����궼Ϊ������꣬��ʼ(0,0)��
	boxFill8(buf, width, COL8_C6C6C6, 0, 0, width - 1, 0);	// ����ɫ (0,0)~(width-1,0)��������
	boxFill8(buf, width, COL8_FFFFFF, 1, 1, width - 2, 1);	// ��ɫ (1,1)~(width-2,1)��������
	boxFill8(buf, width, COL8_C6C6C6, 0, 0, 0, height - 1);	// ����ɫ (0,0)~(0,height-1)�������
	boxFill8(buf, width, COL8_FFFFFF, 1, 1, 1, height - 2);	// ��ɫ (1,1)~(1,height-2)�������
	boxFill8(buf, width, COL8_848484, width - 2, 1, width - 2, height - 2);	//����ɫ (width - 2,1)~(width - 2, height - 2)һ���Ҳ�����
	boxFill8(buf, width, COL8_000000, width - 1, 0, width - 1, height - 1);	// ��ɫ (width - 1,0)~(width - 1, height - 1)һ���Ҳ�����
	boxFill8(buf, width, COL8_C6C6C6, 2, 2, width - 3, height - 3);	// ����ɫ (2,2)~(width - 3,height - 3) ���ľ���
	boxFill8(buf, width, COL8_000084, 3, 3, width - 4, 20);	// ����ɫ(3,3)~(width - 4, 20)��������
	boxFill8(buf, width, COL8_848484, 1, height - 2, width - 2, height - 2);	// ����ɫ(1,height-2)~(width-2,height-2)�ײ�����
	boxFill8(buf, width, COL8_000000, 0, height - 1, width - 1, height - 1);	// ��ɫ(0,height-1)~(width-1,height-1)�ײ�����
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
* Description: ������
*
**********************************************************/
void Main(){
	struct BOOTINFO *binfo;
	char s[40], keyb[32], mouseb[128], timerb[8];	// s����Ҫ����ı�����Ϣ keyb���̻����� mouseb��껺���� timerb��ʱ��������
	int mx, my, bufval; //���x��λ�� ���y��λ�� Ҫ��ʾ�Ļ�������Ϣ
	struct MouseDec mdec;	// ���������Ϣ
	unsigned int memtotal, count = 0;
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;	// �ڴ�α�ָ��
	struct SHTCTL *shtctl;	// ͼ����ƿ�ָ��
	struct SHEET *sheetBack, *sheetMouse, *sheetWin;	// ����ͼ�� ���ͼ�� ����ͼ��
	unsigned char *bufBack, bufMouse[256], *bufWin;	// ����ͼ�񻺳��� ���ͼ�񻺳��� ����ͼ�񻺳���
	struct QUEUE timerbuf;
	
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;	// ��ȡ������Ϣ
	
	initGdtit();	// ��ʼ��GDT IDT
	init_pic();	// ��ʼ���ɱ���жϿ�����
	io_sti();	// ���cpu�жϽ�ֹ
	
	
	QueueInit(&keybuf, 32, keyb);	//��ʼ�����̻���������
	QueueInit(&mousebuf, 128, mouseb);	// ��ʼ����껺��������
	
	
	
	initPit();	// ��ʼ����ʱ��
	io_out8(PIC0_IMR, 0xf8); // ��PIC IRQ0(��ʱ��) IRQ1�����̣���IRQ2����PIC����������(11111000)
	io_out8(PIC1_IMR, 0xef); // ��PIC IRQ12����꣩��������(11101111)
	
	QueueInit(&timerbuf, 8, timerb);	// ��ʼ����ʱ������������
	setTimer(1000, &timerbuf, 1);	// ���ö�ʱ����Ϣ һ��Ҫ��IRQ0�жϿ��ź�
	
	initKeyboard();
	enableMouse(&mdec);	// �������
	
	memtotal = memtest(0x00400000, 0xbfffffff);	// ��ȡ�ڴ��ܺ�
	memsegInit(memsegtable);
	memsegFree(memsegtable, 0x00001000, 0x0009e000);
	memsegFree(memsegtable, 0x00400000, memtotal - 0x00400000);
	
	
	init_palette();	// ��ʼ��16ɫ��ɫ��
	shtctl = shtctlInit(memsegtable, binfo->vram, binfo->scrnx, binfo->scrny);
	sheetBack = sheetAlloc(shtctl);
	sheetMouse = sheetAlloc(shtctl);
	sheetWin = sheetAlloc(shtctl);
	bufBack = (unsigned char *) memsegAlloc4K(memsegtable, binfo->scrnx * binfo->scrny);	// ��4KBΪ��λΪ���������ڴ�
	bufWin = (unsigned char *) memsegAlloc4K(memsegtable, 160 * 52);
	sheetSetbuf(sheetBack, bufBack, binfo->scrnx, binfo->scrny, -1);	// ���ñ���ͼ�㻺��������������Ҫ͸��ɫ��Ϊ-1
	sheetSetbuf(sheetMouse, bufMouse, 16, 16, COL8_008484);	// �������ͼ�㻺������͸��ɫ
	sheetSetbuf(sheetWin, bufWin, 160, 52, -1);	// ���ô���ͼ�㻺���� ��͸��ɫ
	init_GUI(bufBack, binfo->scrnx, binfo->scrny);	// ��ʼ��GUI��bufBack
	initMouseCursor8(bufMouse, COL8_008484);	// ��ʼ�������bufMouse
	makeWindow(bufWin, 160, 52, "counter");	// ��ʾ����
	// putFont8_asc(bufWin, 160, 24, 28, COL8_000000, "Welcome to");
	// putFont8_asc(bufWin, 160, 24, 44, COL8_000000, "DickOS");
	sheetSlide(sheetBack, 0, 0);	// ����ͼ����ʼ����(0,0)
	// ��ʼ�������Ϊ��Ļ����
	mx = (binfo->scrnx - 16) / 2;	// ���x��λ��
	my = (binfo->scrny - 28 - 16) / 2;	// ���y��λ��
	sheetSlide(sheetMouse, mx, my);	// �����ͼ�㻬������Ӧλ��
	sheetSlide(sheetWin, 80, 80);	// ������ͼ�㻬����Ŀ��λ��
	sheetUpdown(sheetBack, 0);	// ������ͼ����������0
	sheetUpdown(sheetWin, 1);	// ������ͼ�����ڱ���֮�����֮��
	sheetUpdown(sheetMouse, 2);	// �����ͼ�����ڱ������Ϸ�����1
	putFont8_asc(bufBack, binfo->scrnx, 30, 32, COL8_FFFFFF, "DickOS");	// ��DickOSд�뱳����
	sprintf(s, "(%d, %d)", mx, my);	// �����λ�ô���s
	putFont8_asc(bufBack, binfo->scrnx, 0, 0, COL8_FFFFFF, s);	// ��sд�뱳��ͼ��
	sprintf(s, "memory %dMB free : %dKB", memtest(0x00400000, 0xbfffffff) / (1024 * 1024), memsegTotal(memsegtable) / 1024);	// ���ڴ���Ϣ����s
	putFont8_asc(bufBack, binfo->scrnx, 0, 64, COL8_FFFFFF, s);	// ��sд�뱳��ͼ��
	sheetRefresh(sheetBack, 0, 0, binfo->scrnx, 64 + 16 + 16 + 76);	// ˢ��ͼ��
	
	//�������������ж������hlt
	for(;;){
		sprintf(s, "%010d", timerctl.count);
		boxFill8(bufWin, 160, COL8_C6C6C6, 40, 28, 119, 43);
		putFont8_asc(bufWin, 160, 40, 28, COL8_000000, s);
		
		sheetRefresh(sheetWin, 40, 28, 120, 44);
		
		io_cli();	// ���ж�
		if(QueueSize(&keybuf) + QueueSize(&mousebuf) + QueueSize(&timerbuf) == 0) {	// ֻ����������������û������ʱ���ܿ����жϲ�����hltģʽ
			io_sti();	// ���ж�
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
				if (mouseDecode(&mdec, bufval) != 0) {	//���һ�������ֽ����ݵĽ��ջ��߳���δ֪���
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					if ((mdec.btn & 0x01) != 0) {	// �������
						s[1] = 'L';
					}
					if ((mdec.btn & 0x02) != 0) {	// �����Ҽ�
						s[3] = 'R';
					}
					if ((mdec.btn & 0x04) != 0) {	// �м����
						s[2] = 'C';
					}
					
					boxFill8(bufBack, binfo->scrnx, COL8_008484, 32, 16, 32 + 15 * 8 - 1, 31);	// �ñ���ɫ����ԭ�������Ϣ
					putFont8_asc(bufBack, binfo->scrnx, 32, 16, COL8_FFFFFF, s);	//��ӡ�����Ϣ
					// ����������ƫ����
					sheetRefresh(sheetBack, 32, 16, 32 + 15 * 8, 32);
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
					boxFill8(bufBack, binfo->scrnx, COL8_008484, 0, 0, 79, 15);	// ����ԭ��������Ϣ
					putFont8_asc(bufBack, binfo->scrnx, 0, 0, COL8_FFFFFF, s);	// ��ʾ�µ�������Ϣ
					sheetRefresh(sheetBack, 0, 0, 80, 16);
					sheetSlide(sheetMouse, mx, my);
				} 
			} else if (QueueSize(&timerbuf) != 0) {
				bufval = QueuePop(&timerbuf);	// ��ȡ��ʱ�����������ж�������
				io_sti();	// ���ж�
				putFont8_asc(bufBack, binfo->scrnx, 0, 80, COL8_FFFFFF, "10[sec]");
				sheetRefresh(sheetBack, 0, 64, 56, 64 + 16 + 16 + 76);
			}
		}
	}
}
