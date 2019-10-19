/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 1.0.4
* @Date: 2019-10-19
* @Description: 函数声明
********************************************************************************/

#ifndef BOOTPACK_H
#define BOOTPACK_H

#include <stdio.h>

/*色号信息*/
#define COL8_000000		0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15

/*描述符信息*/
#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_INTGATE32	0x008e

/*PIC端口信息*/
#define PIC0_ICW1		0x0020	// 写主PIC ICW1的端口地址
#define PIC0_OCW2		0x0020	// 写主PIC OCW2的端口地址
#define PIC0_IMR		0x0021	// 写主PIC 由OCW1写IMR寄存器的端口地址
#define PIC0_ICW2		0x0021	// 写主PIC ICW2的端口地址
#define PIC0_ICW3		0x0021	// 写主PIC ICW3的端口地址
#define PIC0_ICW4		0x0021	// 写主PIC ICW4的端口地址
#define PIC1_ICW1		0x00a0	// 写从PIC ICW1的端口地址
#define PIC1_OCW2		0x00a0	// 写从PIC OCW2的端口地址
#define PIC1_IMR		0x00a1	// 写从PIC 由OCW1写IMR寄存器的端口地址
#define PIC1_ICW2		0x00a1	// 写从PIC ICW2的端口地址
#define PIC1_ICW3		0x00a1	// 写从PIC ICW3的端口地址
#define PIC1_ICW4		0x00a1	// 写从PIC ICW4的端口地址

#define ADR_BOOTINFO	0x00000ff0

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
	short baseLow;	/*基址低地址 16位*/
	char baseMid;	/*基址中地址 8位*/
	char accessRight;	/*段属性低8位（高4位在limitHigh的高4位代表扩展访问权）关于低8位详见note.txt*/
	char limitHigh;	/*limitHigh 段上限高地址 1字节 8位 由于段上限只有20位，所以在limitHigh高4位也写入段的属性*/
	char baseHigh;	/*基址高地址 8位*/
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
void asm_interruptHandler21(void);
void asm_interruptHandler27(void);
void asm_interruptHandler2c(void);

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

/*interrupt.c函数声明*/
void init_pic();
void interruptHandler21(int *esp);
void interruptHandler27(int *esp);
void interruptHandler2c(int *esp);

#endif // BOOTPACK_H
