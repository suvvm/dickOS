/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 0.2.8
* @Date: 2020-01-31
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
	char s[40];
	int	buf[128];	// s����Ҫ����ı�����Ϣ bufΪ�ܻ�����
	int mx, my, bufval, count = 0; //���x��λ�� ���y��λ�� Ҫ��ʾ�Ļ�������Ϣ
	struct MouseDec mdec;	// ���������Ϣ
	unsigned int memtotal;
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;	// �ڴ�α�ָ��
	struct SHTCTL *shtctl;	// ͼ����ƿ�ָ��
	struct SHEET *sheetBack, *sheetMouse, *sheetWin;	// ����ͼ�� ���ͼ�� ����ͼ��
	unsigned char *bufBack, bufMouse[256], *bufWin;	// ����ͼ�񻺳��� ���ͼ�񻺳��� ����ͼ�񻺳���
	struct QUEUE queue;	// �ܻ�����
	struct TIMER *timer1, *timer2, *timer3;	// ������ʱ��ָ��
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;	// ��ȡ������Ϣ
	
	initGdtit();	// ��ʼ��GDT IDT
	init_pic();	// ��ʼ���ɱ���жϿ�����
	io_sti();	// ���cpu�жϽ�ֹ
		
	QueueInit(&queue, 128, buf);	// ��ʼ������������
	
	initPit();	// ��ʼ����ʱ��
	initKeyboard(&queue, 256);	// ��ʼ�����̿��Ƶ�·
	enableMouse(&queue, 512, &mdec);	// �������
	
	io_out8(PIC0_IMR, 0xf8); // ��PIC IRQ0(��ʱ��) IRQ1�����̣���IRQ2����PIC����������(11111000)
	io_out8(PIC1_IMR, 0xef); // ��PIC IRQ12����꣩��������(11101111)
	
	timer1 = timerAlloc();	// ��ȡһ����ʹ�õĶ�ʱ��
	timerInit(timer1, &queue, 10);	// ��ʼ����ʱ��1
	timerSetTime(timer1, 1000);

	timer2 = timerAlloc();	// ��ȡһ����ʹ�õĶ�ʱ��
	timerInit(timer2, &queue, 3);	// ��ʼ����ʱ��1
	timerSetTime(timer2, 300);

	timer3 = timerAlloc();	// ��ȡһ����ʹ�õĶ�ʱ��
	timerInit(timer3, &queue, 1);	// ��ʼ����ʱ��1
	timerSetTime(timer3, 50);
	
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
	sheetSlide(sheetBack, 0, 0);	// ����ͼ����ʼ����(0,0)
	// ��ʼ�������Ϊ��Ļ����
	mx = (binfo->scrnx - 16) / 2;	// ���x��λ��
	my = (binfo->scrny - 28 - 16) / 2;	// ���y��λ��
	sheetSlide(sheetMouse, mx, my);	// �����ͼ�㻬������Ӧλ��
	sheetSlide(sheetWin, 80, 80);	// ������ͼ�㻬����Ŀ��λ��
	sheetUpdown(sheetBack, 0);	// ������ͼ����������0
	sheetUpdown(sheetWin, 1);	// ������ͼ�����ڱ���֮�����֮��
	sheetUpdown(sheetMouse, 2);	// �����ͼ�����ڱ������Ϸ�����1
	
	putFont8AscSheet(sheetBack, 30, 32, COL8_FFFFFF, COL8_008484,  "DickOS", 6);	// ��DickOSд�뱳����
	sprintf(s, "(%3d, %3d)", mx, my);	// �����λ�ô���s
	putFont8AscSheet(sheetBack, 0, 0, COL8_FFFFFF, COL8_008484,  s, 10);	// ��sд�뱳����
	sprintf(s, "memory %dMB free : %dKB", memtest(0x00400000, 0xbfffffff) / (1024 * 1024), memsegTotal(memsegtable) / 1024);	// ���ڴ���Ϣ����s
	putFont8AscSheet(sheetBack, 0, 48, COL8_FFFFFF, COL8_008484,  s, 26);	// ��sд�뱳����
	
	//�����ж������hlt
	for(;;){
		count++;
		// sprintf(s, "%010d", timerctl.count);	
		// putFont8AscSheet(sheetWin, 40, 28, COL8_000000, COL8_C6C6C6,  s, 10);	// ��ʾ��ʱ����ʱ

		io_cli();	// ���ж�
		if(QueueSize(&queue) == 0) {	// ֻ�л�����û������ʱ���ܿ����жϲ�����hltģʽ
			io_sti();	// ���ж�
		} else {
			bufval = QueuePop(&queue);	// ȡ�����������ж�������
			io_sti();	// ���ж�
			if (256 <= bufval && bufval <= 511) {	// �����ж�����
				sprintf(s, "%02X", bufval - 256);
				putFont8AscSheet(sheetBack, 0, 16, COL8_FFFFFF, COL8_008484, s, 2);	// �������ж���Ϣ��ӡ��������
			} else if (512 <= bufval && bufval <= 767) {	// ����ж�����
				if (mouseDecode(&mdec, bufval - 512) != 0) {	// ���һ�������ֽ����ݵĽ��ջ��߳���δ֪���
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
			} else if (bufval == 10) {	// 10�붨ʱ���ж���Ϣ
				putFont8AscSheet(sheetBack, 0, 80, COL8_FFFFFF, COL8_008484,  "10[sec]", 7);	// �ڱ������ӡ10����ʾ	
				sprintf(s, "%010d", count);
				putFont8AscSheet(sheetWin, 40, 28, COL8_000000, COL8_C6C6C6,  s, 11);	// �ڴ��ڲ��ӡcount��ֵ
			} else if (bufval == 3) {	// 3�붨ʱ���ж���Ϣ
				putFont8AscSheet(sheetBack, 0, 96, COL8_FFFFFF, COL8_008484,  "3[sec]", 6);	// �ڱ������ӡ3����ʾ	
				count = 0;
			} else if (bufval == 1) {
				timerInit(timer3, &queue, 0);	// ��ʱ��Ϣ��0
				boxFill8(bufBack, binfo->scrnx, COL8_FFFFFF, 8, 112, 15, 127);
				timerSetTime(timer3, 50);	// ���ó�ʱʱ��0.5��
				sheetRefresh(sheetBack, 8, 112, 16, 128);
			} else if (bufval == 0) {
				timerInit(timer3, &queue, 1);	// ��ʱ��Ϣ��1
				boxFill8(bufBack, binfo->scrnx, COL8_008484, 8, 112, 15, 127);
				timerSetTime(timer3, 50);	// ���ó�ʱʱ��0.5��
				sheetRefresh(sheetBack, 8, 112, 16, 128);
			}
		}
	}
}
