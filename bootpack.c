/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 1.6.1
* @Date: 2019-10-10
* @Description: ����������Ҫʹ�õĹ��ܺ���
********************************************************************************/
#include <bootpack.h>
#include "desctab.c"
#include "graphic.c"

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
