void io_hlt(void);	/*��������*/

void HariMain(void){
	int i;
	char *p;
	for(i = 0xa0000; i <= 0xaffff; i++){
		p = (char *)i;	/*��ָ�����write_mem8�����ڴ�*/
		*p = i & 0x0f;
	}
	for(;;){
		io_hlt();
	}
}
