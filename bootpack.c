void io_hlt(void);	/*函数声明*/
void HariMain(void){
    fin:
        /*执行halt.nas里的io_hlt*/
		io_hlt();
        goto fin;
}
