/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 1.6.1
* @Date: 2019-10-10
* @Description: ����������Ҫʹ�õĹ��ܺ���
********************************************************************************/
#include "desctab.c"
#include "graphic.c"

/*func.nas��������*/
void io_hlt();	
void io_cli();
void io_out8(int port, int data);
int io_load_eflags();
void io_store_eflags(int eflags);
void loadGdtr(int limit, int addr);
void loadIdtr(int limit, int addr);

/*graphic.c��������*/
void init_palette();
void set_palette(int start, int end, unsigned char *rgb);
void boxFill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_GUI(char *vram, int xsize, int ysize);
void putFont8(char *vram, int xsize, int x, int y, char c, char *font);
void putFont8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void initMouseCursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, char *buf, int bxsize);
	
/*desctab.c��������*/
void initGdtit();
void setSegmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void setGatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

/*������Ϣ*/
struct BOOTINFO{	
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char *vram;
};


/*******************************************************
*
* Function name:HariMain
* Description: ������
*
**********************************************************/
void HariMain(){
	struct BOOTINFO *binfo;
	char mcursor[256];
	int mx, my;
	
	initGdtit();
	init_palette();
	binfo = (struct BOOTINFO *) 0xff0;

	mx = (binfo->scrnx - 16) / 2;
	my = (binfo->scrny - 28 - 16) / 2;
	init_GUI(binfo->vram, binfo->scrnx, binfo->scrny);
	putFont8_asc(binfo->vram, binfo->scrnx, 30, 30, COL8_FFFFFF, "SHOWSTRING");
	initMouseCursor8(mcursor, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
	for(;;){
		io_hlt();
	}
}
