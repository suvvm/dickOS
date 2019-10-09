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

void io_hlt();	/*º¯ÊýÉùÃ÷*/
void io_cli();
void io_out8(int port, int data);
int io_load_eflags();
void io_store_eflags(int eflags);

struct BOOTINFO{
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char *vram;
};
void set_palette(int start, int end, unsigned char *rgb){
	int i, eflags;
	eflags = io_load_eflags();	/*±£»¤ÏÖ³¡*/
	io_cli();					/*¹ØÖÐ¶Ï*/
	io_out8(0x03c8, start);
	for(i = start; i < end; i++){
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/*»Ö¸´ÏÖ³¡*/
}
void init_palette(){
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*0:ºÚ*/
		0xff, 0x00, 0x00,	/*1:ÁÁºì*/
		0x00, 0xff, 0x00,	/*2:ÁÁÂÌ*/
		0xff, 0xff, 0x00,	/*3:ÁÁ»Æ*/
		0x00, 0x00, 0xff,	/*4:ÁÁÀ¶*/
		0xff, 0x00, 0xff,	/*5:ÁÁ×Ï*/
		0x00, 0xff, 0xff,	/*6:Ç³ÁÁÀ¶*/
		0xff, 0xff, 0xff,	/*7:°×*/
		0xc6, 0xc6, 0xc6,	/*8:ÁÁ»Ò*/
		0x84, 0x00, 0x00,	/*9:°µºì*/
		0x00, 0x84, 0x00,	/*10:°µÂÌ*/
		0x84, 0x84, 0x00,	/*11:°µ»Æ*/
		0x00, 0x00, 0x84,	/*12:°µÇà*/
		0x84, 0x00, 0x84, 	/*13:°µ×Ï*/
		0x00, 0x84, 0x84,	/*14:Ç³°µÀ¶*/
		0x84, 0x84, 0x84,	/*15:°µ»Ò*/
	};
	set_palette(0, 15, table_rgb);
}
void boxFill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1){
	int x, y;
	for(y = y0; y <= y1; y++){
		for(x = x0; x <= x1; x++){
			vram[y * xsize + x] = c;
		}
	}
}
void putfont8(char *vram, int xsize, int x, int y, char c, char *font){
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
void HariMain(){
	char *vram;
	int xsize, ysize;
	struct BOOTINFO *binfo;
	
	init_palette();
	binfo = (struct BOOTINFO *) 0xff0;
	extern char font[4096];
	init_GUI(binfo->vram, binfo->scrnx, binfo->scrny);
	
	putfont8(binfo->vram, binfo->scrnx,  8, 8, COL8_FFFFFF, font + 'D' * 16);
	putfont8(binfo->vram, binfo->scrnx, 16, 8, COL8_FFFFFF, font + 'I' * 16);
	putfont8(binfo->vram, binfo->scrnx, 24, 8, COL8_FFFFFF, font + 'C' * 16);
	putfont8(binfo->vram, binfo->scrnx, 32, 8, COL8_FFFFFF, font + 'K' * 16);
	putfont8(binfo->vram, binfo->scrnx, 40, 8, COL8_FFFFFF, font + 'O' * 16);
	putfont8(binfo->vram, binfo->scrnx, 48, 8, COL8_FFFFFF, font + 'S' * 16);
	for(;;){
		io_hlt();
	}
}
