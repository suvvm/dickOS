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

/********************************************************************************
* 段描述符存放GDT内容 
* base代表32位段的地址（基址）
* base又分为 low（2字节） mid（1字节） high（1字节）共(2 + 1 + 1) * 8 = 32位
* 32位操作系统段的最大上限位4GB（32位最大数字）
* 但不能直接设为4GB，这样会将32位直接占满，导致没有空间存储段的管理属性信息
* 段的可用上限只有20位，为了解决这个问题inter开发人员在段属性中设置了一个标准位
* Gbit 当Gbit为1时段上限的单位为4KB 4KB * 1MB（20位）= 4GB
*
********************************************************************************/
struct SEGMENT_DESCRIPTOR{
	short limitLow;	/*limitLow 段上限低地址 2字节 16位*/
	char limitHigh;	/*limitHigh 段上限高地址 1字节 8位 由于段上限只有20位，所以在limitHigh高4位也写入段的属性*/
	short baseLow;	/*基址低地址 16位*/
	char baseMid;	/*基址中地址 8位*/
	char baseHigh;	/*基址高地址 8位*/
	char accessRight;	/*段属性低8位（高4位在limitHigh的高4位代表扩展访问权）关于低8位详见note.txt*/
	/*4位扩展访问权由GD00组成 G为Gbit标志位 D为模式位 1代表32位模式 0代表16位模式（即使D为0也不能调用BOIS）*/
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
