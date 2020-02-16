/********************************************************************************
* @File name: helloC.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-08
* @Description: c语言编写的应用程序用于打印hello
********************************************************************************/

#include "apilib.h"

/*******************************************************
*
* Function name: Main
* Description: helloC应用程序主函数
*
**********************************************************/
void Main () {
	apiPutchar('h');
	apiPutchar('e');
	apiPutchar('l');
	apiPutchar('l');
	apiPutchar('o');
	apiEnd();
}
