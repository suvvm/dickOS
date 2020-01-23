/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 0.1.9
* @Date: 2020-01-23
* @Description: ����������Ҫʹ�õĹ��ܺ���
********************************************************************************/
#include "bootpack.h"
#include "desctab.c"
#include "graphic.c"
#include "queue.h"
#include "mouse.c"
#include "memory.c"
#include "sheet.c"

/*******************************************************
*
* Function name:Main
* Description: ������
*
**********************************************************/
void Main(){
	struct BOOTINFO *binfo;
	char s[40], keyb[32], mouseb[128];	// mcursor�����Ϣ s����Ҫ����ı�����Ϣ
	int mx, my, bufval; //���x��λ�� ���y��λ�� Ҫ��ʾ�Ļ�������Ϣ
	struct MouseDec mdec;	// ���������Ϣ
	unsigned int memtotal;
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;	// �ڴ�α�ָ��
	struct SHTCTL *shtctl;	// ͼ����ƿ�ָ��
	struct SHEET *sheetBack, *sheetMouse;	// ����ͼ�� ���ͼ��
	unsigned char *bufBack, bufMouse[256];
	
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;	// ��ȡ������Ϣ
	
	initGdtit();	// ��ʼ��GDT IDT
	init_pic();	// ��ʼ���ɱ���жϿ�����
	io_sti();	// ���cpu�жϽ�ֹ
	
	QueueInit(&keybuf, 32, keyb);	//��ʼ�����̻���������
	QueueInit(&mousebuf, 128, mouseb);	// ��ʼ����껺��������
	
	io_out8(PIC0_IMR, 0xf9); // ��PIC IRQ1�����̣���IRQ2����PIC����������(11111001)
	io_out8(PIC1_IMR, 0xef); // ��PIC IRQ12����꣩��������(11101111)
	
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
	bufBack = (unsigned char *) memsegAlloc4K(memsegtable, binfo->scrnx * binfo->scrny);	// ��4KBΪ��λΪ���������ڴ�
	sheetSetbuf(sheetBack, bufBack, binfo->scrnx, binfo->scrny, -1);	// ���ñ���ͼ�㻺��������������Ҫ͸��ɫ��Ϊ-1
	
	sheetSetbuf(sheetMouse, bufMouse, 16, 16, COL8_008484);	// �������ͼ�㻺������͸��ɫ
	init_GUI(bufBack, binfo->scrnx, binfo->scrny);	// ��ʼ��GUI��bufBack
	initMouseCursor8(bufMouse, COL8_008484);	// ��ʼ�������bufMouse
	
	sheetSlide(sheetBack, 0, 0);	// ����ͼ����ʼ����(0,0)
	// ��ʼ�������Ϊ��Ļ����
	mx = (binfo->scrnx - 16) / 2;	// ���x��λ��
	my = (binfo->scrny - 28 - 16) / 2;	// ���y��λ��
	sheetSlide(sheetMouse, mx, my);	// �����ͼ�㻬������Ӧλ��
	sheetUpdown(sheetBack, 0);	// ������ͼ����������0
	sheetUpdown(sheetMouse, 1);	// �����ͼ�����ڱ������Ϸ�����1
	putFont8_asc(bufBack, binfo->scrnx, 30, 32, COL8_FFFFFF, "DickOS");	// ��DickOSд�뱳����
	sprintf(s, "(%d, %d)", mx, my);	// �����λ�ô���s
	putFont8_asc(bufBack, binfo->scrnx, 0, 0, COL8_FFFFFF, s);	// ��sд�뱳��ͼ��
	sprintf(s, "memory %dMB free : %dKB", memtest(0x00400000, 0xbfffffff) / (1024 * 1024), memsegTotal(memsegtable) / 1024);	// ���ڴ���Ϣ����s
	putFont8_asc(bufBack, binfo->scrnx, 0, 64, COL8_FFFFFF, s);	// ��sд�뱳��ͼ��
	sheetRefresh(sheetBack, 0, 0, binfo->scrnx, 64 + 16 + 16);	// ˢ��ͼ��
	
	//�������������ж������hlt
	for(;;){
		io_cli();
		if(QueueSize(&keybuf) + QueueSize(&mousebuf) == 0) {	// ֻ����������������û������ʱ���ܿ����жϲ�����hltģʽ
			io_stihlt();
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
			}
		}
	}
}
