void io_hlt(void);	/*函数声明*/

void HariMain(void){
	int i;
	char *p;
	p = (char *)i;	/*用指针代替write_mem8操作内存*/
	for(i = 0xa0000; i <= 0xaffff; i++){
		
		*(p + i) = i & 0x0f;
	}
	for(;;){
		io_hlt();
	}
}
