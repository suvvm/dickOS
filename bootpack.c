void io_hlt(void);	/*��������*/

void HariMain(void){
	int i;
	char *p;
	p = (char *)i;	/*��ָ�����write_mem8�����ڴ�*/
	for(i = 0xa0000; i <= 0xaffff; i++){
		
		*(p + i) = i & 0x0f;
	}
	for(;;){
		io_hlt();
	}
}
