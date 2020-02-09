/********************************************************************************
* @File name: helloCstr0.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-09
* @Description: c语言编写的应用程序用调用dickApi打印字符串
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
* Function name: apiPutstr0
* Description: 进行系统调用 调用dickApi显示字符串至0的函数 具体定义在 helloCFunc.nas
* Parameter:
*	@s	要显示的字符串的首地址	char *
*
**********************************************************/
void apiPutstr0(char *s);

/*******************************************************
*
* Function name: Main
* Description: bug1应用程序主函数
*
**********************************************************/
void Main() {
	apiPutstr0("helloC string\n");
	apiEnd();
}
