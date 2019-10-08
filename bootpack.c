void io_hlt();	/*º¯ÊıÉùÃ÷*/
void io_cli();
void io_out8(int port, int data);
int io_load_eflags();
void io_store_eflags(int eflags);




void init_palette();
void set_palette(int start, int end, unsigned char *rgb);

void set_palette(int start, int end, unsigned char *rgb){
	int i, eflags;
	eflags = io_load_eflags();	/*±£»¤ÏÖ³¡*/
	io_cli();					/*¹ØÖĞ¶Ï*/
	io_out8(0x03c8, start);
	for(i = start; i < end; i++){
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/*»Ö¸´ÏÖ³¡*/
	return;
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
	return;
}
void HariMain(){
	int i;
	char *p;	/*ÓÃÖ¸Õë´úÌæwrite_mem8²Ù×÷ÄÚ´æ*/
	
	init_palette();
	
	p = (char *) 0xa0000;
	for(i = 0; i <= 0xffff; i++){
		*(p + i) = i & 0x0f;
	}
	for(;;){
		io_hlt();
	}
}
