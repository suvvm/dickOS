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

void io_hlt();	/*��������*/
void io_cli();
void io_out8(int port, int data);
int io_load_eflags();
void io_store_eflags(int eflags);

void set_palette(int start, int end, unsigned char *rgb){
	int i, eflags;
	eflags = io_load_eflags();	/*�����ֳ�*/
	io_cli();					/*���ж�*/
	io_out8(0x03c8, start);
	for(i = start; i < end; i++){
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/*�ָ��ֳ�*/
	return;
}
void init_palette(){
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*0:��*/
		0xff, 0x00, 0x00,	/*1:����*/
		0x00, 0xff, 0x00,	/*2:����*/
		0xff, 0xff, 0x00,	/*3:����*/
		0x00, 0x00, 0xff,	/*4:����*/
		0xff, 0x00, 0xff,	/*5:����*/
		0x00, 0xff, 0xff,	/*6:ǳ����*/
		0xff, 0xff, 0xff,	/*7:��*/
		0xc6, 0xc6, 0xc6,	/*8:����*/
		0x84, 0x00, 0x00,	/*9:����*/
		0x00, 0x84, 0x00,	/*10:����*/
		0x84, 0x84, 0x00,	/*11:����*/
		0x00, 0x00, 0x84,	/*12:����*/
		0x84, 0x00, 0x84, 	/*13:����*/
		0x00, 0x84, 0x84,	/*14:ǳ����*/
		0x84, 0x84, 0x84,	/*15:����*/
	};
	set_palette(0, 15, table_rgb);
	return;
}
void boxFill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1){
	int x, y;
	for(y = y0; y <= y1; y++){
		for(x = x0; x <= x1; x++){
			vram[y * xsize + x] = c;
		}
	}
	return;
}
void HariMain(){
	int i;
	char *p;	/*��ָ�����write_mem8�����ڴ�*/
	
	init_palette();
	
	p = (char *) 0xa0000;
	
	boxFill8(p, 320, COL8_FF0000, 20, 20, 120, 120);
	boxFill8(p, 320, COL8_00FF00, 70, 50, 170, 150);
	boxFill8(p, 320, COL8_FF00FF, 120, 80, 220, 180);
	for(;;){
		io_hlt();
	}
}
