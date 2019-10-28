/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 1.0.10
* @Date: 2019-10-28
* @Description: ����������Ҫʹ�õĹ��ܺ���
********************************************************************************/
#include "bootpack.h"
#include "desctab.c"
#include "graphic.c"
#include "queue.h"
#include "mouse.c"

/*******************************************************
*
* Function name:Main
* Description: ������
*
**********************************************************/
void Main(){
	struct BOOTINFO *binfo;
	char mcursor[256], s[40], keyb[32], mouseb[128];	// mcursor�����Ϣ s����Ҫ����ı�����Ϣ
	int mx, my, bufval; //���x��λ�� ���y��λ�� Ҫ��ʾ�Ļ�������Ϣ
	unsigned char mouseDbuf[3], mousePhase;
	
	initGdtit();	// ��ʼ��GDT IDT
	init_pic();	// ��ʼ���ɱ���жϿ�����
	io_sti();	// ���cpu�жϽ�ֹ
	
	QueueInit(&keybuf, 32, keyb);	//��ʼ�����̻���������
	QueueInit(&mousebuf, 128, mouseb);	// ��ʼ����껺��������
	
	io_out8(PIC0_IMR, 0xf9); // ����PIC1���ӣ��ͼ���(11111001) 
	io_out8(PIC1_IMR, 0xef); //���(11101111)
	
	initKeyboard();
	
	init_palette();	// ��ʼ��16ɫ��ɫ��
	
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;	// ��ȡ������Ϣ
	mx = (binfo->scrnx - 16) / 2;	// ���x��λ��
	my = (binfo->scrny - 28 - 16) / 2;	// ���y��λ��
	init_GUI(binfo->vram, binfo->scrnx, binfo->scrny);	// ��ʼ��GUI����
	
	// ��ӡDICKOS
	putFont8_asc(binfo->vram, binfo->scrnx, 30, 35, COL8_FFFFFF, "SHOWSTRING");
	initMouseCursor8(mcursor, COL8_008484);	// ��ʼ�������Ϣ
	// ���������Ϣ��ӡ���
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
	sprintf(s, "(%d, %d)", mx, my);	// �����λ�ô���s
	// ��ӡs
	putFont8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
	
	io_out8(PIC0_IMR, 0xf9); // ��PIC IRQ1�����̣���IRQ2����PIC����������(11111001)
	io_out8(PIC1_IMR, 0xef); // ��PIC IRQ12����꣩��������(11101111)
	
	enableMouse();
	mousePhase = 0;	//��ʱΪ�ȴ���괫�ؼ���ظ�0xfa
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
				boxFill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
				putFont8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
			} else {
				bufval = QueuePop(&mousebuf);
				io_sti();
				if (mousePhase == 0) {	//��괫�ؼ���ظ�0xfa
					if (bufval == 0xfa)
						mousePhase = 1;
				} else if (mousePhase == 1) {	// ��괫�ص�һ�ֽ�����
					mouseDbuf[0] = bufval;
					mousePhase = 2;
				} else if (mousePhase == 2) {	// ��괫�صڶ��ֽ�����
					mouseDbuf[1] = bufval;
					mousePhase = 3;
				} else if (mousePhase == 3) {	// ��괫�ص����ֽ�����
					mouseDbuf[2] = bufval;
					mousePhase = 1;	// ׼��������һ���������
					// �����ֽڽ�����ϴ�ӡ����Ϣ
					sprintf(s, "%02X %02X %02X", mouseDbuf[0], mouseDbuf[1], mouseDbuf[2]);
					boxFill8(binfo->vram, binfo->scrnx, COL8_008484, 32, 16, 32 + 8 * 8 - 1, 31);
					putFont8_asc(binfo->vram, binfo->scrnx, 32, 16, COL8_FFFFFF, s);
				} 
			}
		}
		/*
		if (QueueSize(&keybuf) == 0) {
			io_stihlt();
		} else {
			keybufval = QueuePop(&keybuf);
			io_sti();
			sprintf(s, "%02X", keybufval);
			boxFill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
			putFont8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
		}*/
	}
}
