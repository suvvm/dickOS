/********************************************************************************
* @File name: sheet.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-01-22
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
*	@shtclt	图层控制块指针	struct SHTCTL *
*	@sheet	图层指针		struct SHEET *
*	@index	图层索引		int
*
**********************************************************/
void sheetUpdown(struct SHTCTL *shtclt, struct SHEET *sheet, int index) {
	int i, old = sheet->index;	// 保存设置前的索引状态
	if (index > shtclt->top + 1) {	// 指定的索引过高，高于当前最高索引
		index = shtclt->top + 1;	// 修正为当前最高索引加一
	}
	if (index < -1)	{	// 指定的索引过低，低于隐藏
		index = -1;	//修正为隐藏
	}
	sheet->index = index;	// 设定索引
	// 修正sheetsAsc排序
	if (old > index) {	// 若新的索引值小于之前的索引值 将更新的图层降低至目标位置
		if (index >= 0) {	// 非隐藏
			for (i = old; i > index; i--) {
				shtclt->sheetsAcs[i] = shtclt->sheetsAcs[i - 1];
				shtclt->sheetsAcs[i]->index = i;
			}
			shtclt->sheetsAcs[index] = sheet;
		} else {	// 隐藏
			if(shtclt->top > old) {	// 最高索引大于之前的索引值（图层不在最高层）
				for (i = old; i < shtclt->top; i++) {	// 将大于该图层原索引的所有图层的索引降低1
					shtclt->sheetsAcs[i] = shtclt->sheetsAcs[i + 1];
					shtclt->sheetsAcs[i]->index = i;
				}
				shtclt->top--;
			}
		}
		sheetRefresh(shtclt);	// 重新绘制画面
	} else if (old < index) {	// 若新的索引值大于之前的索引值 将更新的图层提升至目标位置
		if (old >= 0) {	// 图层之前未被隐藏
			for (i = old; i < index; i++) {	// 将之前位置到现在位置之间的所有图层索引降低1
				shtclt->sheetsAcs[i] = shtclt->sheetsAcs[i + 1];
				shtclt->sheetsAcs[i]->index = i;
			}
			shtclt->sheetsAcs[index] = sheet;	// 将图层放至对应位置
		} else {	// 图层之前被隐藏
			for (i = shtclt->top; i >= index; i--) {	// 将目标位置之后的所有图层索引提高1
				shtclt->sheetsAcs[i + 1] = shtclt->sheetsAcs[i];
				shtclt->sheetsAcs[i + 1]->index = i + 1;
			}
			shtclt->sheetsAcs[index] = sheet;	// 将图层放至对应位置
			shtclt->top++;
		}
		sheetRefresh(shtclt);
	}
}

/*******************************************************
*
* Function name: sheetRefresh
* Description: 刷新画面（重新绘制图层画面）
* Parameter:
*	@shtclt	图层控制块指针	struct SHTCTL *
*
**********************************************************/
void sheetRefresh(struct SHTCTL *shtclt) {
	int i, sheetX, sheetY, locationX, locationY;
	unsigned char *buf, c, *vram = shtclt->vram;
	struct SHEET *sheet;
	for (i = 0; i <= shtclt->top; i++) {	// 自底向上遍历所有图层
		sheet = shtclt->sheetsAcs[i];
		buf = sheet->buf;
		for(sheetY = 0; sheetY < sheet->height; sheetY++) {
			locationY = sheet->locationY + sheetY;
			for(sheetX = 0; sheetX < sheet->width; sheetX++) {
				locationX = sheet->locationX + sheetX;
				c = buf[sheetY * sheet->width + sheetX];	// 找到目标颜色
				if (c != sheet->colInvNum) {	// 当前要绘制的颜色不是该图层透明色
					vram[locationY * shtclt->xSize + locationX] = c;
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
*	@shtclt		图层控制块指针		struct SHTCTL *
*	@sheet		图层指针			struct SHEET *
*	@locationX	图层新的x轴起始位置	int
*	@locationY	图层新的Y轴起始位置	int
*
**********************************************************/
void sheetSlide(struct SHTCTL *shtclt, struct SHEET *sheet, int locationX, int locationY) {
	sheet->locationX = locationX;
	sheet->locationY = locationY;
	if (sheet->index >= 0) {	// 图层未被隐藏则重新绘制画面
		sheetRefresh(shtclt);
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
