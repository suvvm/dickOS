#ifndef KEYBOARD_C
#define KEYBOARD_C
/********************************************************************************
* @File name: keyboard.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-01-18
* @Description: 键盘设置
********************************************************************************/

#include "bootpack.h"

struct QUEUE *keybuf;

int keyData0;	// 键盘缓冲区数据起点值

/*******************************************************
*
* Function name:waitKeyboardControllerReady
* Description: 等待键盘控制电路准备就绪
*	当从0x0064中读到的数据的倒数第二位为0是表明键盘控制电路准备就绪
*
*******************************************************/
void waitKeyboardControllerReady(){
	// 循环判断控制电路准备就绪
	for (;;) {
		if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0)
			break;
	}
		
}

/*******************************************************
*
* Function name:initKeyboard
* Description: 初始化键盘控制电路模式 
*	等待键盘控制电路就绪后向命令接收端口发送更改模式的指令
*	再次等待键盘控制电路就绪后向模式控制端口发送要更改的模式
* Parameter: 
*	@q		缓冲区队列指针		struct QUEUE *
*	@data0	设置keyData0的值	int
*
*******************************************************/
void initKeyboard(struct QUEUE *q, int data0){
	keybuf = q;
	keyData0 = data0;
	waitKeyboardControllerReady();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	waitKeyboardControllerReady();
	io_out8(PORT_KEYDAT, KBC_MODE);
}

#endif // KEYBOARD_C
