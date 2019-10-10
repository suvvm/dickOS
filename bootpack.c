/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 1.5.9
* @Date: 2019-10-10
* @Description: ����������Ҫʹ�õĹ��ܺ���
********************************************************************************/


#define COL8_000000 0
#define COL8_FF0000 1
#define COL8_00FF00 2
#define COL8_FFFF00 3
#define COL8_0000FF 4
#define COL8_FF00FF 5
#define COL8_00FFFF 6
#define COL8_FFFFFF 7
#define COL8_C6C6C6 8
#define COL8_840000 9
#define COL8_008400 10
#define COL8_848400 11
#define COL8_000084 12
#define COL8_840084 13
#define COL8_008484 14
#define COL8_848484 15

/*func.nas��������*/
void io_hlt();	
void io_cli();
void io_out8(int port, int data);
int io_load_eflags();
void io_store_eflags(int eflags);

/*������Ϣ*/
struct BOOTINFO{	
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char *vram;
};

/*******************************************************
*
* Function name:set_palette
* Description: ���õ�ɫ�壨������ǰ�Ƚ���CLI���ڴ����ָ��жϱ�׼
* Parameter:
* 	@start	��λ����
* 	@end	ĩλ����
* 	@rgb	rgb��ɫ��
**********************************************************/
void set_palette(int start, int end, unsigned char *rgb){
	int i, eflags;
	eflags = io_load_eflags();	/*�����ֳ�*/
	io_cli();					/*���ж�*/
	io_out8(0x03c8, start);
	for(i = start; i < end; i++){
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/*�ָ��ֳ�*/
}

/*******************************************************
*
* Function name:init_palette
* Description: ��ʼ��16ɫ��ɫ��
*
**********************************************************/
void init_palette(){
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*0:��*/
		0xff, 0x00, 0x00,	/*1:����*/
		0x00, 0xff, 0x00,	/*2:����*/
		0xff, 0xff, 0x00,	/*3:����*/
		0x00, 0x00, 0xff,	/*4:����*/
		0xff, 0x00, 0xff,	/*5:����*/
		0x00, 0xff, 0xff,	/*6:ǳ����*/
		0xff, 0xff, 0xff,	/*7:��*/
		0xc6, 0xc6, 0xc6,	/*8:����*/
		0x84, 0x00, 0x00,	/*9:����*/
		0x00, 0x84, 0x00,	/*10:����*/
		0x84, 0x84, 0x00,	/*11:����*/
		0x00, 0x00, 0x84,	/*12:����*/
		0x84, 0x00, 0x84, 	/*13:����*/
		0x00, 0x84, 0x84,	/*14:ǳ����*/
		0x84, 0x84, 0x84,	/*15:����*/
	};
	set_palette(0, 15, table_rgb);
}

/*******************************************************
*
* Function name:boxFill8
* Description: ���ƾ���
* Parameter:
* 	@vram	�Դ�ָ��
* 	@xsize	��Ļ��ȷֱ���
* 	@c	��ɫ���(0~15)
* 	@x0	x����ʼλ��
* 	@y0	y����ʼλ��
* 	@x1	x�����λ��
* 	@y1	y�����λ��
*
**********************************************************/
void boxFill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1){
	int x, y;
	for(y = y0; y <= y1; y++){
		for(x = x0; x <= x1; x++){
			vram[y * xsize + x] = c;
		}
	}
}

/*******************************************************
*
* Function name:putFont8
* Description: ����Ļ�д�ӡ�����ַ�
* Parameter:
* 	@vram	�Դ�ָ��
* 	@xsize	��Ļ��ȷֱ���
* 	@x	x����ʼλ��
* 	@y	y����ʼλ��
* 	@c	��ɫ���(0~15)
* 	@font	��������
*
**********************************************************/
void putFont8(char *vram, int xsize, int x, int y, char c, char *font){
	int i;
	char *p, d;
	for(i = 0; i < 16; i++){
		p = vram + (y + i) * xsize + x;
		d = font[i];
		if((d & 0x80) != 0) p[0] = c;
		if((d & 0x40) != 0) p[1] = c;
		if((d & 0x20) != 0) p[2] = c;
		if((d & 0x10) != 0) p[3] = c;
		if((d & 0x08) != 0) p[4] = c;
		if((d & 0x04) != 0) p[5] = c;
		if((d & 0x02) != 0) p[6] = c;
		if((d & 0x01) != 0) p[7] = c;
	}
}

/*******************************************************
*
* Function name:init_GUI
* Description: ��������ͼ��
* Parameter:
* 	@vram	�Դ�ָ��
* 	@xsize	��Ļ��ȷֱ���
* 	@ysize	��Ļ�߶ȷֱ���
*
**********************************************************/
void init_GUI(char *vram, int xsize, int ysize){
	boxFill8(vram, xsize, COL8_008484,  0,         0,          xsize -  1, ysize - 29);
	boxFill8(vram, xsize, COL8_C6C6C6,  0,         ysize - 28, xsize -  1, ysize - 28);
	boxFill8(vram, xsize, COL8_FFFFFF,  0,         ysize - 27, xsize -  1, ysize - 27);
	boxFill8(vram, xsize, COL8_C6C6C6,  0,         ysize - 26, xsize -  1, ysize -  1);

	boxFill8(vram, xsize, COL8_FFFFFF,  3,         ysize - 24, 59,         ysize - 24);
	boxFill8(vram, xsize, COL8_FFFFFF,  2,         ysize - 24,  2,         ysize -  4);
	boxFill8(vram, xsize, COL8_848484,  3,         ysize -  4, 59,         ysize -  4);
	boxFill8(vram, xsize, COL8_848484, 59,         ysize - 23, 59,         ysize -  5);
	boxFill8(vram, xsize, COL8_000000,  2,         ysize -  3, 59,         ysize -  3);
	boxFill8(vram, xsize, COL8_000000, 60,         ysize - 24, 60,         ysize -  3);

	boxFill8(vram, xsize, COL8_848484, xsize - 47, ysize - 24, xsize -  4, ysize - 24);
	boxFill8(vram, xsize, COL8_848484, xsize - 47, ysize - 23, xsize - 47, ysize -  4);
	boxFill8(vram, xsize, COL8_FFFFFF, xsize - 47, ysize -  3, xsize -  4, ysize -  3);
	boxFill8(vram, xsize, COL8_FFFFFF, xsize -  3, ysize - 24, xsize -  3, ysize -  3);
}

/*******************************************************
*
* Function name:putFont8_asc
* Description: ����Ļ�д�ӡ�ַ���
* Parameter:
* 	@vram	�Դ�ָ��
* 	@xsize	��Ļ��ȷֱ���
* 	@x	x����ʼλ��
* 	@y	y����ʼλ��
* 	@c	��ɫ���(0~15)
* 	@s	�ַ�����λ
*
**********************************************************/
void putFont8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s){
	extern char font[4096];
	for(; *s != 0x00; s++){
		putFont8(vram, xsize, x, y, c, font + *s * 16);
		x += 8;
	}
}

/*******************************************************
*
* Function name:initMouseCursor8
* Description: ��ʼ�����ͼ��
* Parameter:
* 	@mouse	�洢�����Ϣ��λ����λ
* 	@bc	����ɫ
*
**********************************************************/
void initMouseCursor8(char *mouse, char bc){
	
	static char cursor[16][16] = {
		"*...............",
		"**..............",
		"*O*.............",
		"*OO*............",
		"*OOO*...........",
		"*OOOO*..........",
		"*OOOOO*.........",
		"*OOOOOO*........",
		"*OOOOOOO*.......",
		"*OOOO*****......",
		"*OO*O*..........",
		"*O*.*O*.........",
		"**..*O*.........",
		"*....*O*........",
		".....*O*........",
		"......*........."
	};
	int x, y;
	for(y = 0; y < 16; y++){
		for(x = 0; x < 16; x++){
			if(cursor[y][x] == '*')
				mouse[y * 16 + x] = COL8_000000;
			if(cursor[y][x] == 'O')
				mouse[y * 16 + x] = COL8_FFFFFF;
			if(cursor[y][x] == '.')
				mouse[y * 16 + x] = bc;
		}
	}
}

/*******************************************************
*
* Function name:putblock8_8
* Description: ��ʼ�����ͼ��
* Parameter:
* 	@vram	�洢�����Ϣ��λ����λ
* 	@vxsize	����ɫ
* 	@pxsize	���ָ����
* 	@pysize	���ָ��߶�
* 	@px0	x����ʼλ��
* 	@py0	y����ʼλ��
* 	@buf	���ͼ����Ϣ��λ
* 	@bxsize	���ͼ����Ϣ��ָ��Ŀ��
*
**********************************************************/
void putblock8_8(char *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, char *buf, int bxsize)
{
	int x, y;
	for (y = 0; y < pysize; y++) {
		for (x = 0; x < pxsize; x++) {
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
	return;
}

/*******************************************************
*
* Function name:HariMain
* Description: ������
*
**********************************************************/
void HariMain(){
	char *vram;
	int xsize, ysize;
	struct BOOTINFO *binfo;
	char mcursor[256];
	int mx, my;
	
	
	init_palette();
	binfo = (struct BOOTINFO *) 0xff0;
	extern char font[4096];
	mx = (binfo->scrnx - 16) / 2;
	my = (binfo->scrny - 28 - 16) / 2;
	init_GUI(binfo->vram, binfo->scrnx, binfo->scrny);
	
	putFont8(binfo->vram, binfo->scrnx,  8, 8, COL8_FFFFFF, font + 'D' * 16);
	putFont8(binfo->vram, binfo->scrnx, 16, 8, COL8_FFFFFF, font + 'I' * 16);
	putFont8(binfo->vram, binfo->scrnx, 24, 8, COL8_FFFFFF, font + 'C' * 16);
	putFont8(binfo->vram, binfo->scrnx, 32, 8, COL8_FFFFFF, font + 'K' * 16);
	putFont8(binfo->vram, binfo->scrnx, 40, 8, COL8_FFFFFF, font + 'O' * 16);
	putFont8(binfo->vram, binfo->scrnx, 48, 8, COL8_FFFFFF, font + 'S' * 16);
	putFont8_asc(binfo->vram, binfo->scrnx, 30, 30, COL8_FFFFFF, "SHOWSTRING");
	
	initMouseCursor8(mcursor, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
	for(;;){
		io_hlt();
	}
}
