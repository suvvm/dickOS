/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 1.5.9
* @Date: 2019-10-10
* @Description: 包含启动后要使用的功能函数
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

/*func.nas函数声明*/
void io_hlt();	
void io_cli();
void io_out8(int port, int data);
int io_load_eflags();
void io_store_eflags(int eflags);
void loadGdtr(int limit, int addr);
void loadIdtr(int limit, int addr);

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

/*******************************************************
*
* Function name:setSegmdesc
* Description: 设置指定GDT段描述符
* Parameter:
* 	@sd	指向要修改段描述符内存首地址的指针
* 	@limit	段描述符所描述内存段基于段基址最大偏移量
* 	@base	段描述符所描述内存段基址
* 	@ar	段描述符特权级,类型等
*
**********************************************************/
void setSegmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar){
	if(limit > 0xfffff){	/*内存段长度超过2^20B(1MB) 将该段定义为4G空间*/
		ar |= 0x8000;	/*G_bit = 1*/
		limit /= 0x1000;	/*单位换算为4KB*/
	}
	sd->limitLow = limit & 0xffff;	/*取limit低16位*/
	sd->baseLow = base & 0xffff;	/*取base低16位*/
	sd->baseMid = (base >> 16) & 0xff;	/*右移16位取低8位*/
	sd->accessRight = ar & 0xff;	/*取属性ar的低8位*/
	sd->limitHigh = ((limit >> 16) & 0xff) | ((ar >> 8) & 0xf0);	/*取limit右移16位后取低4位（就是取limit高4位） | 属性右移8位后取低8位（ar高8位）*/
	sd->baseHigh = (base >> 24) & 0xff;	/*base右移24位后取低8位（base高8位）*/
}

/*******************************************************
*
* Function name:setSegmdesc
* Description: 设置指定IDT描述符
* Parameter:
* 	@gd	指向要修改IDT描述符内存首地址的指针
* 	@offset	IDT描述符所在段的偏移地址
* 	@selector	处理程序所在内存段的段符
* 	@ar	IDT描述符特权级,类型等
*
**********************************************************/
void setGatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar){
	gd->offsetLow = offset & 0xffff;	/*取offset低16位*/
	gd->selector = selector;	/*段选择符*/
	gd->accessRight = ar & 0xff;	/*取ar低8位*/
	gd->dwCount = (ar >> 8) & 0xff;		/*ar右移8位后取低8位*/
	gd->offsetHigh = (offset >> 16) & 0xffff;	/*offset右移16位后取低16位（取offset高16位）*/
}
/*******************************************************
*
* Function name:initGdtit
* Description: 初始化GDT,IDT
*
**********************************************************/
void initGdtit(){
	/*段描述符表GDT地址为 0x270000~0x27ffff*/
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) 0x00270000;
	/*中断描述符表IDT地址为 0x26f800~0x26ffff*/
	struct GATE_DESCRIPTOR *idt = (struct GATE_DESCRIPTOR *) 0x0026f800;
	int i;
	/*将GDT内存段（全8192个）初始化为0*/
	for(i = 0; i < 8192; i++){
		setSegmdesc(gdt + i, 0, 0, 0);
	}
	/*将段号为1的段上限（基于段基址最大偏移量）设为4GB，基址是0，表示的是32位下cpu能管理的全部内存，段属性为0x4092*/
	setSegmdesc(gdt + 1, 0xffffffff, 0x00000000, 0x4092);
	/*将段号为2的段上限设为512KB，基址为0x00280000，为执行bootpack.hrb的段*/
	setSegmdesc(gdt + 2, 0x0007ffff, 0x00280000, 0x409a);
	/*调用汇编函数向gdtr赋值（将信息加载给寄存器）*/
	loadGdtr(0xffff, 0x00270000);
	/*将IDT描述符（全256个）初始化为0*/
	for(i = 0; i < 256; i++){
		setGatedesc(idt + i, 0, 0, 0);
	}
	/*调用汇编函数向idtr赋值（将信息加载给寄存器）*/
	loadIdtr(0x7ff, 0x0026f800);
}

/*******************************************************
*
* Function name:set_palette
* Description: 设置调色板（在设置前先进行CLI并在处理后恢复中断标准）
* Parameter:
* 	@start	首位号码
* 	@end	末位号码
* 	@rgb	rgb颜色码
**********************************************************/
void set_palette(int start, int end, unsigned char *rgb){
	int i, eflags;
	eflags = io_load_eflags();	/*保护现场*/
	io_cli();					/*关中断*/
	io_out8(0x03c8, start);
	for(i = start; i < end; i++){
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/*恢复现场*/
}

/*******************************************************
*
* Function name:init_palette
* Description: 初始化16色调色板
*
**********************************************************/
void init_palette(){
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*0:黑*/
		0xff, 0x00, 0x00,	/*1:亮红*/
		0x00, 0xff, 0x00,	/*2:亮绿*/
		0xff, 0xff, 0x00,	/*3:亮黄*/
		0x00, 0x00, 0xff,	/*4:亮蓝*/
		0xff, 0x00, 0xff,	/*5:亮紫*/
		0x00, 0xff, 0xff,	/*6:浅亮蓝*/
		0xff, 0xff, 0xff,	/*7:白*/
		0xc6, 0xc6, 0xc6,	/*8:亮灰*/
		0x84, 0x00, 0x00,	/*9:暗红*/
		0x00, 0x84, 0x00,	/*10:暗绿*/
		0x84, 0x84, 0x00,	/*11:暗黄*/
		0x00, 0x00, 0x84,	/*12:暗青*/
		0x84, 0x00, 0x84, 	/*13:暗紫*/
		0x00, 0x84, 0x84,	/*14:浅暗蓝*/
		0x84, 0x84, 0x84,	/*15:暗灰*/
	};
	set_palette(0, 15, table_rgb);
}

/*******************************************************
*
* Function name:boxFill8
* Description: 绘制矩形
* Parameter:
* 	@vram	显存指针
* 	@xsize	屏幕宽度分辨率
* 	@c	颜色标号(0~15)
* 	@x0	x轴起始位置
* 	@y0	y轴起始位置
* 	@x1	x轴结束位置
* 	@y1	y轴结束位置
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
* Description: 在屏幕中打印单个字符
* Parameter:
* 	@vram	显存指针
* 	@xsize	屏幕宽度分辨率
* 	@x	x轴起始位置
* 	@y	y轴起始位置
* 	@c	颜色标号(0~15)
* 	@font	字体数据
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
* Description: 绘制桌面图像
* Parameter:
* 	@vram	显存指针
* 	@xsize	屏幕宽度分辨率
* 	@ysize	屏幕高度分辨率
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
* Description: 在屏幕中打印字符串
* Parameter:
* 	@vram	显存指针
* 	@xsize	屏幕宽度分辨率
* 	@x	x轴起始位置
* 	@y	y轴起始位置
* 	@c	颜色标号(0~15)
* 	@s	字符串首位
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
* Description: 初始化鼠标图像
* Parameter:
* 	@mouse	存储鼠标信息的位置首位
* 	@bc	背景色
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
* Description: 初始化鼠标图像
* Parameter:
* 	@vram	存储鼠标信息的位置首位
* 	@vxsize	背景色
* 	@pxsize	鼠标指针宽度
* 	@pysize	鼠标指针高度
* 	@px0	x轴起始位置
* 	@py0	y轴起始位置
* 	@buf	鼠标图像信息首位
* 	@bxsize	鼠标图像信息中指针的宽度
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
* Description: 主函数
*
**********************************************************/
void HariMain(){
	struct BOOTINFO *binfo;
	char mcursor[256];
	int mx, my;
	
	initGdtit();
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
