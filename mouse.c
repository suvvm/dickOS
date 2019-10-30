/********************************************************************************
* @File name: keyboard.c
* @Author: suvvm
* @Version: 1.0.4
* @Date: 2019-10-30
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
* Parameter:
* 	@mdec	保存鼠标信息的结构体
*******************************************************/
void enableMouse(struct MouseDec *mdec) {
	waitKeyboardControllerReady();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	waitKeyboardControllerReady();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	mdec->phase = 0;	// 等待接收鼠标激活成功反馈码
}

/*******************************************************
*
* Function name:mouseDecode
* Description: 鼠标信息设置
*	鼠标键状态在buf[0]的低三位
*	鼠标x与y基本直接使用buf[1]与buf[2]
*	但是如果鼠标正在移动要将其第8位及以后的位设为全0或全1
*	因为鼠标移动时对x轴移动与y轴移动有反应
*	鼠标与屏幕y轴相反，所以要做反转y轴操作
* Parameter:
* 	@mdec	保存鼠标信息的结构体
*	@data	当前收到的鼠标信息
*
*******************************************************/
int mouseDecode(struct MouseDec *mdec, unsigned char data) {
	if (mdec->phase == 0) { // 当前还未处理鼠标传回的激活回复0xfa
		if (data == 0xfa) {	// 收到激活回复，准备处理鼠标第一段信息
			mdec->phase = 1;
		}
		// 如果是其他什么信息就直接丢掉
		return 0;
	}
	if (mdec->phase == 1) {	// 接收鼠标第一段信息
		if ((data & 0xc8) == 0x08) {
			// 鼠标第一字节的高四位在鼠标移动时会在0~3范围内发生改变，而第四位在鼠标按键时会在8~F发生改变
			// 所以要对其进行检测 检测其对应范围是否正确
			mdec->buf[0] = data;	// 记录第一字节数据
			mdec->phase = 2;	// 准备处理第二字节数据
		}
		return 0;
	}
	if (mdec->phase == 2) {	// 接收鼠标第二段信息
		mdec->buf[1] = data;	// 记录第二字节数据
		mdec->phase = 3;	// 准备接收第三字节数据
		return 0;
	}
	if (mdec->phase == 3) {	// 接收鼠标第三段信息并进行数据解读
		mdec->buf[2] = data;	// 记录第三字节数据
		mdec->phase = 1;	// 接收完了一波数据，下一波将再次从第一字节开始接收
		mdec->btn = mdec->buf[0] & 0x07;	// 鼠标键状态在buf[0]的低三位
		mdec->x = mdec->buf[1];
		mdec->y = mdec->buf[2];
		if ((mdec->buf[0] & 0x10) != 0) {	// 鼠标正在x轴移动
			mdec->x |= 0xffffff00;
		}
		if ((mdec->buf[0] & 0x20) != 0) {	// 鼠标正在y轴移动
			mdec->y |= 0xffffff00;
		}
		mdec->y = -mdec->y;	//反转y轴
		return 1;
	}
	// 按理说phase会去到其他值，也就是不会到达这里
	return -1;
}
