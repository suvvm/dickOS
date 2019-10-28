/********************************************************************************
* @File name: keyboard.c
* @Author: suvvm
* @Version: 1.0.2
* @Date: 2019-10-28
* @Description: 键盘设置
********************************************************************************/

#include "bootpack.h"
#include "keyboard.c"

/*******************************************************
*
* Function name:enableMouse
* Description: 激活鼠标
*	当键盘控制电路收到指令0xd4则下一条收到的数据将会发送给鼠标
*	向鼠标发送0xf4激活鼠标 鼠标将会向cpu产生中断发送答复数据0xfa
*
*******************************************************/
void enableMouse() {
	waitKeyboardControllerReady();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	waitKeyboardControllerReady();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
}
