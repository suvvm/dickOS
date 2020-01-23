/********************************************************************************
* @File name: sheet.c
* @Author: suvvm
* @Version: 0.0.2
* @Date: 2020-01-23
* @Description: 定义图层相关函数
********************************************************************************/
#include "bootpack.h"

/*******************************************************
*
* Function name:	shtctlInit
* Description:	初始化图层控制块
* Parameter:
*	@memsegtable	段表指针		struct MEMSEGTABLE *
*	@vram			对应vram地址	unsigned char *
*	@xSize			vram画面宽度	int
*	@ySize			vram画面高度	int
* Return:
*	成功返回指向图层控制块地址的指针，失败返回0
*
**********************************************************/
struct SHTCTL *shtctlInit(struct MEMSEGTABLE *memsegtable, unsigned char *vram, int xSize, int ySize) {
	struct SHTCTL *shtctl;
	int i;
	shtctl = (struct SHTCTL *) memsegAlloc4K(memsegtable, sizeof(struct SHTCTL));	// 计算SHTCTL所需内存并以4KB为单位进行分配
	if (shtctl == 0) {	// 内存分配失败
		return shtctl;
	}
	shtctl->vram = vram;
	shtctl->xSize = xSize;
	shtctl->ySize = ySize;
	shtctl->top = -1;	// 初始状态一个图层都没有，顶层图层索引记为-1
	for (i = 0; i < MAX_SHEETS; i++) {
		shtctl->sheets[i].status = 0;	// 所有图层标记为未使用
	}
	return shtctl;
}

/*******************************************************
*
* Function name:	sheetAlloc
* Description:	取得新生成的未使用图层
* Parameter:
*	@shtctl	图层控制块指针	struct SHTCTL *
* Return:
*	成功返回指向对应图层的指针，失败返回0
*
**********************************************************/
struct SHEET *sheetAlloc(struct SHTCTL *shtctl) {
	struct SHEET *sheet;
	int i;
	for (i = 0; i < MAX_SHEETS; i++) {
		if (shtctl->sheets[i].status == 0) {	// 找到未使用的图层
			sheet = &shtctl->sheets[i];
			sheet->status = SHEET_USE;	// 标记为正在使用
			sheet->index = -1;	// 隐藏（表示该图层并未设置索引，需要设置）
			return sheet;
		}
	}
	return 0;	// 所有图层都已被使用
}

/*******************************************************
*
* Function name: sheetSetbuf
* Description: 设置图层缓冲区的大小和透明色(加载图层内容)
* Parameter:
*	@sheet		图层指针	struct SHEET *
*	@buf		缓冲区指针	unsigned char *
*	@width		图层宽度	int
*	@height		图层高度	int
*	@colInvNum	透明色号	int
*
**********************************************************/
void sheetSetbuf(struct SHEET *sheet, unsigned char *buf, int width, int height, int colInvNum) {
	sheet->buf = buf;
	sheet->width = width;
	sheet->height = height;
	sheet->colInvNum = colInvNum;
}

/*******************************************************
*
* Function name: sheetUpdown
* Description: 设置图层索引
* Parameter:
*	@shtctl	图层控制块指针	struct SHTCTL *
*	@sheet	图层指针		struct SHEET *
*	@index	图层索引		int
*
**********************************************************/
void sheetUpdown(struct SHTCTL *shtctl, struct SHEET *sheet, int index) {
	int i, old = sheet->index;	// 保存设置前的索引状态
	if (index > shtctl->top + 1) {	// 指定的索引过高，高于当前最高索引
		index = shtctl->top + 1;	// 修正为当前最高索引加一
	}
	if (index < -1)	{	// 指定的索引过低，低于隐藏
		index = -1;	//修正为隐藏
	}
	sheet->index = index;	// 设定索引
	// 修正sheetsAsc排序
	if (old > index) {	// 若新的索引值小于之前的索引值 将更新的图层降低至目标位置
		if (index >= 0) {	// 非隐藏
			for (i = old; i > index; i--) {
				shtctl->sheetsAcs[i] = shtctl->sheetsAcs[i - 1];
				shtctl->sheetsAcs[i]->index = i;
			}
			shtctl->sheetsAcs[index] = sheet;
		} else {	// 隐藏
			if(shtctl->top > old) {	// 最高索引大于之前的索引值（图层不在最高层）
				for (i = old; i < shtctl->top; i++) {	// 将大于该图层原索引的所有图层的索引降低1
					shtctl->sheetsAcs[i] = shtctl->sheetsAcs[i + 1];
					shtctl->sheetsAcs[i]->index = i;
				}
				shtctl->top--;
			}
		}
		sheetRefresh(shtctl, sheet, sheet->locationX, sheet->locationY, sheet->locationX + sheet->width, sheet->locationY + sheet->height);	// 重新绘制画面
	} else if (old < index) {	// 若新的索引值大于之前的索引值 将更新的图层提升至目标位置
		if (old >= 0) {	// 图层之前未被隐藏
			for (i = old; i < index; i++) {	// 将之前位置到现在位置之间的所有图层索引降低1
				shtctl->sheetsAcs[i] = shtctl->sheetsAcs[i + 1];
				shtctl->sheetsAcs[i]->index = i;
			}
			shtctl->sheetsAcs[index] = sheet;	// 将图层放至对应位置
		} else {	// 图层之前被隐藏
			for (i = shtctl->top; i >= index; i--) {	// 将目标位置之后的所有图层索引提高1
				shtctl->sheetsAcs[i + 1] = shtctl->sheetsAcs[i];
				shtctl->sheetsAcs[i + 1]->index = i + 1;
			}
			shtctl->sheetsAcs[index] = sheet;	// 将图层放至对应位置
			shtctl->top++;
		}
		sheetRefresh(shtctl, sheet, sheet->locationX, sheet->locationY, sheet->locationX + sheet->width, sheet->locationY + sheet->height);
	}
}

/*******************************************************
*
* Function name: sheetRefresh
* Description: 刷新指定图层指定位置的画面
* Parameter:
*	@shtctl	图层控制块指针					struct SHTCTL *
*	@sheet	图层指针						struct SHEET *
*	@startX	指定图层欲刷新部分x轴起始位置	int
*	@startY	指定图层欲刷新部分y轴起始位置	int
*	@endX	指定图层欲刷新部分x轴终点位置	int
*	@endY	指定图层欲刷新部分y轴终点位置	int
*
**********************************************************/
void sheetRefresh(struct SHTCTL *shtctl, struct SHEET *sheet, int startX, int startY, int endX, int endY) {
	/* sheetRefreshAll
	int i, sheetX, sheetY, locationX, locationY;
	unsigned char *buf, c, *vram = shtctl->vram;
	struct SHEET *sheet;
	for (i = 0; i <= shtctl->top; i++) {	// 自底向上遍历所有图层
		sheet = shtctl->sheetsAcs[i];
		buf = sheet->buf;
		for(sheetY = 0; sheetY < sheet->height; sheetY++) {
			locationY = sheet->locationY + sheetY;
			for(sheetX = 0; sheetX < sheet->width; sheetX++) {
				locationX = sheet->locationX + sheetX;
				c = buf[sheetY * sheet->width + sheetX];	// 找到目标颜色
				if (c != sheet->colInvNum) {	// 当前要绘制的颜色不是该图层透明色
					vram[locationY * shtctl->xSize + locationX] = c;
				}
			}
		}
	}
	*/
	if (sheet->index >= 0) {	// 图层未被隐藏
		sheetRefreshSub(shtctl, sheet->locationX + startX, sheet->locationY + startY, sheet->locationX + endX, sheet->locationY + endY);	// 刷新vram指定位置
	}
}

/*******************************************************
*
* Function name: sheetRefreshSub
* Description: 刷新varm指定范围内的画面
* Parameter:
*	@shtctl		图层控制块指针			struct SHTCTL *
*	@startX		欲刷新区域x轴起始坐标	int
*	@startY		欲刷新区域y轴起始坐标	int
*	@endX		欲刷新区域的宽度		int
*	@endY		欲刷新区域的高度		int
*
**********************************************************/
void sheetRefreshSub(struct SHTCTL *shtctl, int startX, int startY, int endX, int endY) {
	int i, sheetX, sheetY, locationX, locationY;
	unsigned char *buf, c, *vram = shtctl->vram;
	struct SHEET *sheet;
	for (i = 0; i <= shtctl->top; i++){
		sheet = shtctl->sheetsAcs[i];
		buf = sheet->buf;
		for (sheetY = 0; sheetY < sheet->height; sheetY++) {
			locationY = sheet->locationY + sheetY;	// 找到图像在vram y轴映射的位置
			for (sheetX = 0; sheetX < sheet->width; sheetX++) {
				locationX = sheet->locationX + sheetX;	// 找到图像在vram x轴映射的位置
				if(startX <= locationX && locationX < endX && startY <= locationY && locationY < endY) {	// 判断是否在欲刷新区域内
					c = buf[sheetY * sheet->width + sheetX];
					if (c != sheet->colInvNum) {	// c不为图层透明色
						vram[locationY * shtctl->xSize + locationX] = c;
					}
				}
			}
		}
	}
}

/*******************************************************
*
* Function name: sheetSlide
* Description: 不改变图层索引，图层在原索引层位置变更
* Parameter:
*	@shtctl		图层控制块指针		struct SHTCTL *
*	@sheet		图层指针			struct SHEET *
*	@locationX	图层新的x轴起始位置	int
*	@locationY	图层新的Y轴起始位置	int
*
**********************************************************/
void sheetSlide(struct SHTCTL *shtctl, struct SHEET *sheet, int locationX, int locationY) {
	int oldLocationX = sheet->locationX, oldLocationY = sheet->locationY;
	sheet->locationX = locationX;
	sheet->locationY = locationY;
	if (sheet->index >= 0) {	// 图层未被隐藏则重新绘制画面
		sheetRefreshSub(shtctl, oldLocationX, oldLocationY, oldLocationX + sheet->width, oldLocationY + sheet->height);	// 刷新移动前区域
		sheetRefreshSub(shtctl, locationX, locationY, locationX + sheet->width, locationY + sheet->height);	// 刷新移动目标区域
		
		//sheetRefresh(shtctl);
	}
}

/*******************************************************
*
* Function name:	sheetFree
* Description:	释放目标图层
* Parameter:
*	@shtctl	图层控制块指针	struct SHTCTL *
*	@sheet	图称指针		struct SHEET *
*
**********************************************************/
void sheetFree(struct SHTCTL *shtctl, struct SHEET *sheet) {
	if (sheet->index >= 0) {	// 如果图层仍在显示则先将其隐藏
		sheetUpdown(shtctl, sheet, -1);
	}
	sheet->status = 0;	// 标记该图层为未使用
}
