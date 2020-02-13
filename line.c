/********************************************************************************
* @File name: line.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-10
* @Description: c语言编写的应用程序，使用malloc申请内存空间，显示窗口随机绘制多条直线 手动刷新图层
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

/*******************************************************
*
* Function name: apiLineWin
* Description: 在图层中绘制直线
* Parameter:
*	@sheet	图层句柄	int
*	@startX	x轴起始位置	int
*	@startY	y轴起始位置	int
*	@endX	x轴截止位置	int
*	@endY	y轴截止位置	int
*	@col	色号		int
*
**********************************************************/
void apiLineWin(int sheet, int startX, int startY, int endX, int endY, int col);

/*******************************************************
*
* Function name: apiEnd
* Description: 进行系统调用 调用dickApi结束应用程序 具体定义在 helloCFunc.nas
*
**********************************************************/
void apiEnd();

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

void Main() {
	char *buf;
	int win, i;
	apiInitMalloc();
	buf = apiMalloc(160 * 100);
	win = apiOpenWindow(buf, 160, 100, -1, "line");
	for (i = 0; i < 8; i++) {
		apiLineWin(win + 1, 8, 26, 77, i * 9 + 26, i);
		apiLineWin(win + 1, 88, 26, i * 9 + 88, 89, i);
	}
	apiRefreshWin(win, 6, 26, 154, 90);	// 手动刷新图层
	for (;;) {
		if (apiGetKey(1) == 0x0a) {
			break;
		}
	}
	apiEnd();
}
