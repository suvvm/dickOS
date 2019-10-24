/********************************************************************************
* @File name: keyboard.c
* @Author: suvvm
* @Version: 1.0.1
* @Date: 2019-10-24
* @Description: 键盘设置
********************************************************************************/

#include "bootpack.h"

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
*
*******************************************************/
void initKeyboard(){
	waitKeyboardControllerReady();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	waitKeyboardControllerReady();
	io_out8(PORT_KEYDAT, KBC_MODE);
}
