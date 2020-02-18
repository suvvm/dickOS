/********************************************************************************
* @File name: apilib.h
* @Author: suvvm
* @Version: 0.0.2
* @Date: 2020-02-18
* @Description: 存放api函数声明
********************************************************************************/
#ifndef APILIB_H
#define APILIB_H

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
* Function name: apiPutstr0
* Description: 进行系统调用 调用dickApi显示字符串至0的函数 具体定义在 helloCFunc.nas
* Parameter:
*	@s	要显示的字符串的首地址	char *
*
**********************************************************/
void apiPutstr0(char *s);

/*******************************************************
*
* Function name: apiPutstr1
* Description: 进行系统调用 调用dickApi显示指定长度字符串的函数 具体定义在 helloCFunc.nas
* Parameter:
*	@s	要显示的字符串的首地址	char *
*	@l	要显示的长度			int
*
**********************************************************/
void apiPutstr1(char *s, int l);

/*******************************************************
*
* Function name: apiEnd
* Description: 进行系统调用 调用dickApi结束应用程序 具体定义在 helloCFunc.nas
*
**********************************************************/
void apiEnd();

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
* Function name: apiBeep
* Description: 蜂鸣器发声
* Parameter:
*	@tone	声音频率	int
*
**********************************************************/
void apiBeep(int tone);

/*******************************************************
*
* Function name: apiFileOpen
* Description: 打开文件
* Parameter:
*	@fileName	文件名	char *
*
**********************************************************/
int apiFileOpen(char *fileName);

/*******************************************************
*
* Function name: apiFileClose
* Description: 关闭文件
* Parameter:
*	@fileHandle	文件句柄	int
*
**********************************************************/
void apiFileClose(int fileHandle);

/*******************************************************
*
* Function name: apiFileSeek
* Description: 文件定位 模式0起点文件开头 模式1 起点为当前访问位置
*				模式2 起点为文件结尾
* Parameter:
*	@fileHandle	文件句柄	int
*	@offset		定位偏移量	char *
*	@mode		定位模式	int
*
**********************************************************/
void apiFileSeek(int fileHandle, int offset, int mode);

/*******************************************************
*
* Function name: apiFileSize
* Description: 获取文件大小 模式0文件大小 模式1 文件首到当前位置的大小
*				模式2 文件当前位置到文件末尾的大小
* Parameter:
*	@fileHandle	文件句柄	int
*	@mode		获取模式	int
* Return:
*	返回读取后的文件大小
*
**********************************************************/
int apiFileSize(int fileHandle, int mode);

/*******************************************************
*
* Function name: apiFileRead
* Description: 读取文件
* Parameter:
*	@buf		目标缓冲区指针	char *
*	@maxSize	最大读取字节数	int
*	@fileHandle	文件句柄		int
* Return:
*	返回读取后的文件内容当前位置
*
**********************************************************/
int apiFileRead(char *buf,int maxSize, int fileHandle);

/*******************************************************
*
* Function name: apiCmdline
* Description: 读取命令行数据
* Parameter:
*	@buf		命令行数据地址	char *
*	@maxSize	最大读取字节数	int
*
**********************************************************/
int apiCmdline(char *buf, int maxSize);

#endif
