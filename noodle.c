/********************************************************************************
* @File name: noodle.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-11
* @Description: c语言编写的应用程序 定时器计时显示时分秒
********************************************************************************/
#include <stdio.h>
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
* Function name: apiInitMalloc 
* Description: 调用dickApi功能号8读取应用程序文件数据并初始化（memsegtable）
*
**********************************************************/
void apiInitMalloc();

/*******************************************************
*
* Function name: apiMalloc
* Description: 分配指定大小的内存空间
* Parameter:
*	@size		欲分配空间的大小		int
*
**********************************************************/
char *apiMalloc(int size);

/*******************************************************
*
* Function name: apiFree
* Description: 释放指定地址指定大小的内存空间
* Parameter:
*	@addr		欲释放地址	char *
*	@size		欲释放大小	int
*
**********************************************************/
void apiFree(char *addr, int size);

/*******************************************************
*
* Function name: apiPoint
* Description: 在指定图层指定位置打印点
* Parameter:
*	@sheet	图层句柄	int
*	@x		x位置		int
*	@y		y位置		int
*	@col	色号		col
*
**********************************************************/
void apiPoint(int sheet, int x, int y, int col);

/*******************************************************
*
* Function name: apiCloseWin
* Description: 关闭窗口
* Parameter:
*	@sheet	图层句柄	int
*
**********************************************************/
void apiCloseWin(int sheet);

/*******************************************************
*
* Function name: apiGetKey
* Description: 获取键盘输入
* Parameter:
*	@mode	模式0没有键盘输入时返回-1不休眠	int
*			模式1休眠直到发生键盘中断
*
**********************************************************/
int apiGetKey(int mode);

/*******************************************************
*
* Function name: apiAllocTimer
* Description: 分配定时器
* Return:
*	返回获取的定时器的句柄
*
**********************************************************/
int apiAllocTimer();

/*******************************************************
*
* Function name: apiInitTimer
* Description: 初始化定时器
* Parameter:
*	@timer	定时器句柄				int
*	@data	要设置的定时器超时数据	int
*
**********************************************************/
void apiInitTimer(int timer, int data);

/*******************************************************
*
* Function name: apiSetTimer
* Description: 设置定时器超时时限
* Parameter:
*	@timer	定时器句柄	int
*	@time	超时时限	int
*
**********************************************************/
void apiSetTimer(int timer, int time);

/*******************************************************
*
* Function name: apiFreeTimer
* Description: 释放定时器
* Parameter:
*	@timer	定时器句柄	int
*
**********************************************************/
void apiFreeTimer(int timer);

/*******************************************************
*
* Function name: apiEnd
* Description: 进行系统调用 调用dickApi结束应用程序 具体定义在 helloCFunc.nas
*
**********************************************************/
void apiEnd();

/*******************************************************
*
* Function name: apiRefreshWin
* Description: 手动刷新图层
* Parameter:
*	@sheet	图层句柄	int
*	@startX	x轴起始位置	int
*	@startY	y轴起始位置	int
*	@endX	x轴截止位置	int
*	@endY	y轴截止位置	int
*
**********************************************************/
void apiRefreshWin(int sheet, int startX, int startY, int endX, int endY);


void Main() {
	char *buf, s[12];
	int win, timer, sec = 0, min = 0, hou = 0;	// 窗口句柄 定时器句柄 秒 分 时
	apiInitMalloc();
	buf = apiMalloc(150 * 50);
	win = apiOpenWindow(buf, 150, 50, -1, "noodle");
	timer = apiAllocTimer();
	apiInitTimer(timer, 128);	// 超时数据注意不要与键盘数据冲突
	for (;;) {
		sprintf(s, "%5d:%02d:%02d", hou, min, sec);
		apiBoxFillWin(win, 28, 27, 115, 41, 7);	// 白色
		apiPutStrWin(win, 28, 27, 0, 11, s);	// 黑色
		apiSetTimer(timer, 100);	// 1s
		if (apiGetKey(1) != 128) {	// 任意按键退出
			break;
		}
		sec++;
		if (sec == 60) {
			sec = 0;
			min++;
			if (min == 60) {
				min = 0;
				hou++;
			}
		}
	}
	apiEnd();
}
