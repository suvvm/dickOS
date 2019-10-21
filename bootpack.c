/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 1.0.4
* @Date: 2019-10-21
* @Description: ����������Ҫʹ�õĹ��ܺ���
********************************************************************************/
#include "bootpack.h"
#include "desctab.c"
#include "graphic.c"

/*******************************************************
*
* Function name:HariMain
* Description: ������
*
**********************************************************/
void HariMain(){
	struct BOOTINFO *binfo;
	char mcursor[256], s[40];	// mcursor�����Ϣ s����Ҫ����ı�����Ϣ
	int mx, my, keybufval;
	
	initGdtit();	// ��ʼ��GDT IDT
	init_pic();	// ��ʼ���ɱ���жϿ�����
	
	io_sti();	// ���cpu�жϽ�ֹ
	
	init_palette();	// ��ʼ��16ɫ��ɫ��
	
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;	// ��ȡ������Ϣ
	mx = (binfo->scrnx - 16) / 2;	// ���x��λ��
	my = (binfo->scrny - 28 - 16) / 2;	// ���y��λ��
	init_GUI(binfo->vram, binfo->scrnx, binfo->scrny);	// ��ʼ��GUI����
	// ��ӡDICKOS
	putFont8_asc(binfo->vram, binfo->scrnx, 30, 30, COL8_FFFFFF, "SHOWSTRING");
	initMouseCursor8(mcursor, COL8_008484);	// ��ʼ�������Ϣ
	// ���������Ϣ��ӡ���
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
	sprintf(s, "(%d, %d)", mx, my);	// �����λ�ô���s
	// ��ӡs
	putFont8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
	
	io_out8(PIC0_IMR, 0xf9); // ��PIC IRQ1�����̣���IRQ2����PIC����������(11111001)
	io_out8(PIC1_IMR, 0xef); // ��PIC IRQ12����꣩��������(11101111)
	
	//��������ж������hlt
	for(;;){
		io_cli();
		if (keybuf.flag == 0) {
			io_stihlt();
		} else {
			keybufval = keybuf.data;
			keybuf.flag = 0;
			io_sti();
			sprintf(s, "%02X", keybufval);
			boxFill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
			putFont8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
		}
	}
}
