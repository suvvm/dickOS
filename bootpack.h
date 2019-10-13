/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 1.0.1
* @Date: 2019-10-13
* @Description: 函数声明
********************************************************************************/

#ifndef BOOTPACK_H
#define BOOTPACK_H

/*启动信息*/
struct BOOTINFO{	
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char *vram;
};

/*段描述符存放GDT内容*/
struct SEGMENT_DESCRIPTOR{
	short limitLow, baseLow;
	char baseMid, accessRight;
	char limitHigh, baseHigh;
};

/*门描述符存放IDT内容*/
struct GATE_DESCRIPTOR{
	short offsetLow, selector;
	char dwCount, accessRight;
	short offsetHigh;
};

/*func.nas函数声明*/
void io_hlt();	
void io_cli();
void io_out8(int port, int data);
int io_load_eflags();
void io_store_eflags(int eflags);
void loadGdtr(int limit, int addr);
void loadIdtr(int limit, int addr);

/*graphic.c函数声明*/
void init_palette();
void set_palette(int start, int end, unsigned char *rgb);
void boxFill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_GUI(char *vram, int xsize, int ysize);
void putFont8(char *vram, int xsize, int x, int y, char c, char *font);
void putFont8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void initMouseCursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, char *buf, int bxsize);
	
/*desctab.c函数声明*/
void initGdtit();
void setSegmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void setGatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

#endif // BOOTPACK_H
