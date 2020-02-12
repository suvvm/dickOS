/********************************************************************************
* @File name: color2.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-11
* @Description: c语言编写的应用程序 用6级色阶模拟21级色阶
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
* Function name: apiEnd
* Description: 进行系统调用 调用dickApi结束应用程序 具体定义在 helloCFunc.nas
*
**********************************************************/
void apiEnd();

unsigned char rgb2Pal(int r, int g, int b, int x, int y) {
	static int table[4] = {3, 1, 0, 2};
	int i;
	// 判断奇偶
	x &= 1;
	y &= 1;
	i = table[x + y * 2];
	r = (r * 21) / 256;
	g = (g * 21) / 256;
	b = (b * 21) / 256;
	r = (r + i) / 4;
	g = (g + i) / 4;
	b = (b + i) / 4;
	return 16 + r + g * 6 + b * 36;
}

void Main() {
	char *buf;
	int win, x, y;
	apiInitMalloc();
	buf = apiMalloc(144 * 164);
	win = apiOpenWindow(buf, 144, 164, -1, "color");
	for (y = 0; y < 128; y++) {
		for (x = 0; x < 128; x++) {
			buf[(x + 8) + (y + 28) * 144] = rgb2Pal(x * 2, y * 2, 0, x, y);
		}
	}
	apiRefreshWin(win, 8, 28, 136, 156);
	apiGetKey(1);
	apiEnd();
}

