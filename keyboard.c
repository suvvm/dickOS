#ifndef KEYBOARD_C
#define KEYBOARD_C
/********************************************************************************
* @File name: keyboard.c
* @Author: suvvm
* @Version: 0.0.3
* @Date: 2020-02-02
* @Description: 键盘设置
********************************************************************************/

#include "bootpack.h"

static char keyboardTable[0x80] = {
	 0,   0,  '1', '2', '3', '4', '5', '6',  '7',  '8', '9',  '0',  '-', '=',  0,   0,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',  'O',  'P', '[',  ']',   0,   0,  'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'',  '`',  0,  '\\',  'Z', 'X', 'C', 'V',
	'B', 'N', 'M', ',', '.', '/',  0,  '*',   0,   ' ',  0,    0,    0,   0,   0,   0,
	 0,   0,   0,   0,   0,   0,   0,  '7',  '8',  '9', '-',  '4',  '5', '6', '+', '1',
	'2', '3', '0', '.',  0,   0,   0,   0,    0,    0,   0,    0,    0,   0,   0,   0,
	 0,   0,   0,   0,   0,   0,   0,   0,    0,    0,   0,    0,    0,   0,   0,   0,
	 0,   0,   0,   0,   0,   0,   0,   0,    0,    0,   0,    0,    0,   0,   0,   0
};

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
