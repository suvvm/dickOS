#ifndef WINDOW_C
#define WINDOW_C
/********************************************************************************
* @File name: window.c
* @Author: suvvm
* @Version: 0.0.2
* @Date: 2020-02-11
* @Description: 实现窗口相关函数
********************************************************************************/

#include "bootpack.h"

/*******************************************************
*
* Function name: makeTextBox
* Description: 创建文本输入框
* Parameter:
*	@sheet	图称指针	struct SHEET *
*	@startX	x轴起始位置	int
*	@startY	y轴起始位置	int
*	@width	宽度		int
*	@height	高度		int
*	@c		文本框颜色	int
*
**********************************************************/
void makeTextBox(struct SHEET *sheet, int startX, int startY, int width, int height, int c) {
	int endX = startX + width, endY = startY + height;
	boxFill8(sheet->buf, sheet->width, COL8_848484, startX - 2, startY - 3, endX + 1, startY - 3);	// 暗灰色 (startX-2,startY-3)~(endX+1,startY-3) 一条上部边框横线
	boxFill8(sheet->buf, sheet->width, COL8_848484, startX - 3, startY - 3, startX - 3, endY + 1);	// 暗灰色 (startX-3,startY-3)~(startX-3,endY+1) 一条左侧边框竖线
	boxFill8(sheet->buf, sheet->width, COL8_FFFFFF, startX - 3, endY + 2, endX + 1, endY + 2);		// 白色 (startX-3,endY+2)~(endX+1,endY+2) 一条底部边框横线
	boxFill8(sheet->buf, sheet->width, COL8_FFFFFF, endX + 2, startY - 3, endX + 2, endY + 2);		// 白色 (endX+2,startY-2)~(endX+2,endY+2) 一条右侧边框竖线
	boxFill8(sheet->buf, sheet->width, COL8_000000, startX - 1, startY - 2, endX, startY - 2);		// 黑色	(startX-1,startY-2)~(endX,startY-2) 一条顶部边框横线
	boxFill8(sheet->buf, sheet->width, COL8_000000, startX - 2, startY - 2, startX - 2, endY);		// 黑色 (startX-2,startY-2)~(startX-2,endY) 一条左侧边框竖线
	boxFill8(sheet->buf, sheet->width, COL8_C6C6C6, startX - 2, endY + 1, endX, endY + 1);			// 亮灰色 (startX-2,endY+1,endX,endY+1) 一条底部边框横线
	boxFill8(sheet->buf, sheet->width, COL8_C6C6C6, endX + 1, startY - 2, endX + 1, endY + 1);		// 亮灰色 (endX+1,startY-2)~(endX+1,endY+1) 一条右侧边框竖线
	
	boxFill8(sheet->buf, sheet->width, c, startX - 1, startY - 1, endX, endY);	// 文本输入区
}

/*******************************************************
*
* Function name: makeWindowTitle
* Description: 绘制窗口标题栏
* Parameter:
*	@buf	窗口图像缓冲区	unsigned char *
*	@width	窗口宽度		int
*	@title	窗口标题		char *
*	@act	活动标识		char
*
**********************************************************/
void makeWindowTitle(unsigned char *buf, int width, char *title, char act) {
	static char closeBtn[14][16] = {	// 关闭按钮
		"OOOOOOOOOOOOOOO@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQQQ@@QQQQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"O$$$$$$$$$$$$$$@",
		"@@@@@@@@@@@@@@@@"
	};
	int x, y;
	char c, tc, tbc;	// closeBtn对应位置字符 title文字颜色 窗口标题栏颜色
	if (act != 0) {	// 窗口为活动窗口
		tc = COL8_FFFFFF;	// 白色
		tbc = COL8_000084;	// 暗青色
	} else {	// 窗口不为活动窗口
		tc = COL8_C6C6C6;	// 亮灰色
		tbc = COL8_848484;	// 暗灰色
	}
	boxFill8(buf, width, tbc, 3, 3, width - 4, 20);	// tbc(3,3)~(width - 4, 20)标题矩形
	putFont8_asc(buf, width, 24, 4, tc, title);	// 标题文字
		for (y = 0; y < 14; y++) {
		for (x = 0; x < 16; x++) {
			c = closeBtn[y][x];
			if (c == '@') {
				c = COL8_000000;
			} else if (c == '$') {
				c = COL8_848484;
			} else if (c == 'Q') {
				c = COL8_C6C6C6;
			} else {
				c = COL8_FFFFFF;
			}
			buf[(5 + y) * width + (width - 21 + x)] = c; 
		}
	}
}

/*******************************************************
*
* Function name: makeWindow
* Description: 创建窗口
* Parameter:
*	@buf	窗口图像缓冲区	unsigned char *
*	@width	窗口宽度		int
*	@height	窗口高度		int
*	@title	窗口标题		char *
*	@act	活动标识		char
*
**********************************************************/
void makeWindow(unsigned char *buf, int width, int height, char *title, char act) {
	
	// 绘制窗口（坐标都为相对坐标，起始(0,0)）
	boxFill8(buf, width, COL8_C6C6C6, 0, 0, width - 1, 0);	// 亮灰色 (0,0)~(width-1,0)顶部横线
	boxFill8(buf, width, COL8_FFFFFF, 1, 1, width - 2, 1);	// 白色 (1,1)~(width-2,1)顶部横线
	boxFill8(buf, width, COL8_C6C6C6, 0, 0, 0, height - 1);	// 亮灰色 (0,0)~(0,height-1)左侧竖线
	boxFill8(buf, width, COL8_FFFFFF, 1, 1, 1, height - 2);	// 白色 (1,1)~(1,height-2)左侧竖线
	boxFill8(buf, width, COL8_848484, width - 2, 1, width - 2, height - 2);	//暗灰色 (width - 2,1)~(width - 2, height - 2)一条右侧竖线
	boxFill8(buf, width, COL8_000000, width - 1, 0, width - 1, height - 1);	// 黑色 (width - 1,0)~(width - 1, height - 1)一条右侧竖线
	boxFill8(buf, width, COL8_C6C6C6, 2, 2, width - 3, height - 3);	// 亮灰色 (2,2)~(width - 3,height - 3) 中心矩形
	boxFill8(buf, width, COL8_848484, 1, height - 2, width - 2, height - 2);	// 暗灰色(1,height-2)~(width-2,height-2)底部横线
	boxFill8(buf, width, COL8_000000, 0, height - 1, width - 1, height - 1);	// 黑色(0,height-1)~(width-1,height-1)底部横线
	makeWindowTitle(buf, width, title, act);
}

/*******************************************************
*
* Function name: changeWinTitle
* Description: 修改窗口标题栏状态
* Parameter:
*	@sheet	窗口图层指针	struct SHEET *
*	@act	活动标识		char
*
**********************************************************/
void changeWinTitle(struct SHEET *sheet, char act) {
	int x, y, width = sheet->width;
	char c, tcOld, tbcOld, tcNew, tbcNew;	// closeBtn对应位置字符 title文字颜色旧 窗口标题栏颜色旧 title文字颜色新 窗口标题栏颜色新
	if (act != 0) {	// 窗口为活动窗口
		tcNew = COL8_FFFFFF;	// 白色
		tbcNew = COL8_000084;	// 暗青色
		tcOld = COL8_C6C6C6;	// 亮灰色
		tbcOld = COL8_848484;	// 暗灰色
	} else {	// 窗口不为活动窗口
		tcNew = COL8_C6C6C6;	// 亮灰色
		tbcNew = COL8_848484;	// 暗灰色
		tcOld = COL8_FFFFFF;	// 白色
		tbcOld = COL8_000084;	// 暗青色
	}
	for (y = 3; y < 20; y++) {	// 将老颜色转换为新颜色
		for (x = 3; x < width - 4; x++) {
			c = sheet->buf[y * width + x];
			if (c == tcOld && x <= width - 22) {
				c = tcNew;
			} else if (c == tbcOld) {
				c = tbcNew;
			}
			sheet->buf[y * width + x] = c; 
		}
	}
	sheetRefresh(sheet, 3, 3, width, 21);
}

#endif	// WINDOW_C
