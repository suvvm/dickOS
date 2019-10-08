void io_hlt();	/*��������*/
void io_cli();
void io_out8(int port, int data);
int io_load_eflags();
void io_store_eflags(int eflags);




void init_palette();
void set_palette(int start, int end, unsigned char *rgb);

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
void HariMain(){
	int i;
	char *p;	/*��ָ�����write_mem8�����ڴ�*/
	
	init_palette();
	
	p = (char *) 0xa0000;
	for(i = 0; i <= 0xffff; i++){
		*(p + i) = i & 0x0f;
	}
	for(;;){
		io_hlt();
	}
}
