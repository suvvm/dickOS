/********************************************************************************
* @File name: graphic.c
* @Author: suvvm
* @Version: 0.0.3
* @Date: 2020-01-18
* @Description: 包含绘制图像要使用的功能函数
********************************************************************************/
#include "bootpack.h"

/*******************************************************
*
* Function name:set_palette
* Description: 设置调色板（在设置前先进行CLI并在处理后恢复中断标准）
* Parameter:
* 	@start	首位号码
* 	@end	末位号码
* 	@rgb	rgb颜色码
*
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
* Description: 显示图像
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
