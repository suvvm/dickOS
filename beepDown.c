/********************************************************************************
* @File name: beepDown.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-12
* @Description: c语言编写的应用程序 使蜂鸣器发声
*	每隔0.01秒降低发声频率 至20Hz或用户按键停止
********************************************************************************/
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

void Main() {
	int i, timer;
	timer = apiAllocTimer();
	apiInitTimer(timer, 128);
	
	for (i = 20000000; i >= 20000; i -= i / 100) {
		// 20KHz ~ 20Hz人耳识别范围 以%1的速度递减
		apiBeep(i);
		apiSetTimer(timer, 1); // 0.01s
		if (apiGetKey(1) != 128) {
			break;
		}
	}
	apiBeep(0);	// 关闭蜂鸣器
	apiEnd();
}
