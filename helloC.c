/********************************************************************************
* @File name: helloC.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-08
* @Description: c语言编写的应用程序用于打印hello
********************************************************************************/

/*******************************************************
*
* Function name: apiPutchar
* Description: 进行系统调用 调用dickApi显示字符的函数 具体定义在 helloCFunc.nas
* Parameter:
*	@c	要显示的字符ascii码	int
*
**********************************************************/
void apiPutchar(int c);

/*******************************************************
*
* Function name: apiEnd
* Description: 进行系统调用 调用dickApi结束应用程序 具体定义在 helloCFunc.nas
*
**********************************************************/
void apiEnd();

/*******************************************************
*
* Function name: Main
* Description: helloC应用程序主函数
*
**********************************************************/
void Main() {
	apiPutchar('h');
	apiPutchar('e');
	apiPutchar('l');
	apiPutchar('l');
	apiPutchar('o');
	apiEnd();
}
