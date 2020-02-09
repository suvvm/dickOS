/********************************************************************************
* @File name: winHelo2.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-09
* @Description: c语言编写的应用程序用于显示窗口并显示字符绘制矩形
********************************************************************************/

/*******************************************************
*
* Function name: apiOpenWindow
* Description: 设置图层缓冲区的大小和透明色(加载图层内容)
* Parameter:
*	@buf		缓冲区指针	char *
*	@width		图层宽度	int
*	@height		图层高度	int
*	@colInvNum	透明色号	int
*	@title		窗口标题	char *
*
**********************************************************/
int apiOpenWindow(char *buf, int width, int height, int colInvNum, char *title);

/*******************************************************
*
* Function name: apiPutStrWin
* Description: 在窗口图层打印字符串
* Parameter:
*	@win		窗口图层		int
*	@x			x轴起始位置		int
*	@y			y轴起始位置		int
*	@col		色号			int
*	@len		字符串长度		int
*	@str		字符串首地址	char *
*
**********************************************************/
void apiPutStrWin(int win, int x, int y, int col, int len, char *str);

/*******************************************************
*
* Function name: apiBoxFillWin
* Description: 在窗口图层绘制矩形
* Parameter:
*	@win		窗口图层		int
*	@startX		x轴起始位置		int
*	@startY		y轴起始位置		int
*	@endX		x轴截止位置		int
*	@endY		y轴截止位置		int
*	@col		色号			int
*
**********************************************************/
void apiBoxFillWin(int win, int startX, int startY, int endX, int endY, int col);

/*******************************************************
*
* Function name: apiEnd
* Description: 进行系统调用 调用dickApi结束应用程序 具体定义在 helloCFunc.nas
*
**********************************************************/
void apiEnd();

char buf[150 * 50];

void Main() {
	int win;
	win = apiOpenWindow(buf, 150, 50, -1, "hello");
	apiBoxFillWin(win, 8, 36, 141, 43, 3);	// 黄色
	apiPutStrWin(win, 28, 28, 0, 12, "hello, world");
	apiEnd();
}
