/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 1.0.13
* @Date: 2019-11-11
* @Description: 包含启动后要使用的功能函数
********************************************************************************/
#include "bootpack.h"
#include "desctab.c"
#include "graphic.c"
#include "queue.h"
#include "mouse.c"


/*******************************************************
*
* Function name:memtestSub
* Description: 调查start到end地址范围内能够使用的内存的末尾地址
* Parameter:
*	@start	需要调查的起始地址
*	@end	需要调查的末尾地址
* Return:
*	返回能够使用的末尾地址
*
**********************************************************/
unsigned int memtestSub(unsigned int start, unsigned int end) {
	unsigned int i, *p, old, pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;
	for(i = start; i <= end; i += 0x1000) {	// 对于每个要检查的内存进行写入测试看看是否能读出正确的值
		p = (unsigned int *)(i + 0xffc);	// 检查4KB末尾4字节
		old = *p;	// 记录先前修改的值
		*p = pat0;	// 试写
		*p ^= 0xffffffff;	// 与全1按位异或（反转）进行反转的原因是如果直接读出的话有些机型会直接读出要写入的数据，起不到检查的效果
		if(*p != pat1) {
notMemoey:
			*p = old;
			break;
		}
		*p ^= 0xffffffff;	// 再次反转
		if(*p != pat0) {	// 检查再次反转后值是否恢复为初值
			goto notMemoey;
		}
		*p = old;
	}
	return i;
}

/*******************************************************
*
* Function name: memtest
* Description: 检查CPU是否在486以上（386没有cache）
*	如果CPU在486以上就将缓存设为OFF，之后调用具体处理函数获得内存可以使用的末尾地址
* Parameter:
*	@start	需要调查的起始地址
*	@end	需要调查的末尾地址
* Return:
*	返回能够使用的末尾地址
**********************************************************/
unsigned int memtest(unsigned int start, unsigned int end) {
	char flag486 = 0;
	unsigned int eflag, cr0, i;
	eflag = io_load_eflags();	// 读取当前eflags的值
	eflag |= EFLAGS_AC_BIT;		// 手动将eflags的AC位设为1
	io_store_eflags(eflag);	// 写入eflags
	eflag = io_load_eflags();	//再次读取eflags的值
	if((eflag & EFLAGS_AC_BIT) != 0) {	// 如果是386，即使手动设为1AC也会恢复为0
		flag486 = 1;	// 如果AC位不为零则一定在386以上
	}
	eflag &= ~EFLAGS_AC_BIT;	// 将eflagsAC位恢复为0
	io_store_eflags(eflag);	// 写入eflags
	
	if(flag486 != 0) {	// 386以上
		cr0 = loadCr0();	// 读取当前cr0寄存器的值
		cr0 |= CR0_CACHE_DISABLE;	// 禁止缓存	
		storeCr0(cr0);	// 写入cr0寄存器
	}
	
	i = memtestSub(start, end);
	
	if(flag486 != 0) {	// 386以上
		cr0 = loadCr0();	// 读取当前cr0寄存器的值
		cr0 &= ~CR0_CACHE_DISABLE;	// 允许缓存	
		storeCr0(cr0);	// 写入cr0寄存器
	}
	
	return i;
}

/*******************************************************
*
* Function name:Main
* Description: 主函数
*
**********************************************************/
void Main(){
	struct BOOTINFO *binfo;
	char mcursor[256], s[40], keyb[32], mouseb[128];	// mcursor鼠标信息 s保存要输出的变量信息
	int mx, my, bufval, i; //鼠标x轴位置 鼠标y轴位置 要显示的缓冲区信息
	struct MouseDec mdec;	// 保存鼠标信息
	
	initGdtit();	// 初始化GDT IDT
	init_pic();	// 初始化可编程中断控制器
	io_sti();	// 解除cpu中断禁止
	
	QueueInit(&keybuf, 32, keyb);	//初始化键盘缓冲区队列
	QueueInit(&mousebuf, 128, mouseb);	// 初始化鼠标缓冲区队列
	
	io_out8(PIC0_IMR, 0xf9); // 主PIC IRQ1（键盘）与IRQ2（从PIC）不被屏蔽(11111001)
	io_out8(PIC1_IMR, 0xef); // 从PIC IRQ12（鼠标）不被控制(11101111)
	
	initKeyboard();
	
	init_palette();	// 初始化16色调色板
	
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;	// 获取启动信息
	mx = (binfo->scrnx - 16) / 2;	// 鼠标x轴位置
	my = (binfo->scrny - 28 - 16) / 2;	// 鼠标y轴位置
	init_GUI(binfo->vram, binfo->scrnx, binfo->scrny);	// 初始化GUI背景
	
	// 打印DICKOS
	putFont8_asc(binfo->vram, binfo->scrnx, 30, 32, COL8_FFFFFF, "DickOS");
	initMouseCursor8(mcursor, COL8_008484);	// 初始化鼠标信息
	// 根据鼠标信息打印鼠标
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
	sprintf(s, "(%d, %d)", mx, my);	// 将鼠标位置存入s
	// 打印s
	putFont8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
	
	i = memtest(0x00400000, 0xbfffffff) / (1024 * 1024);
	sprintf(s, "memory %dMB", i);
	putFont8_asc(binfo->vram, binfo->scrnx, 0, 64, COL8_FFFFFF, s);
	
	enableMouse(&mdec);	// 激活鼠标
	//处理键盘与鼠标中断与进入hlt
	for(;;){
		io_cli();
		if(QueueSize(&keybuf) + QueueSize(&mousebuf) == 0) {	// 只有在两个缓冲区都没有数据时才能开启中断并进入hlt模式
			io_stihlt();
		} else {
			if (QueueSize(&keybuf) != 0) {
				bufval = QueuePop(&keybuf);
				io_sti();
				sprintf(s, "%02X", bufval);
				boxFill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
				putFont8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
			} else if (QueueSize(&mousebuf) != 0) {
				bufval = QueuePop(&mousebuf);
				io_sti();
				if (mouseDecode(&mdec, bufval) != 0) {	//完成一波三个字节数据的接收或者出现未知差错
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					if ((mdec.btn & 0x01) != 0) {	// 按下左键
						s[1] = 'L';
					}
					if ((mdec.btn & 0x02) != 0) {	// 按下右键
						s[3] = 'R';
					}
					if ((mdec.btn & 0x04) != 0) {	// 中间滚轮
						s[2] = 'C';
					}
					
					boxFill8(binfo->vram, binfo->scrnx, COL8_008484, 32, 16, 32 + 15 * 8 - 1, 31);	// 用背景色覆盖原有鼠标信息
					putFont8_asc(binfo->vram, binfo->scrnx, 32, 16, COL8_FFFFFF, s);	//打印鼠标信息
					// 鼠标移动操作
					boxFill8(binfo->vram, binfo->scrnx, COL8_008484, mx, my, mx + 15, my + 15);	// 以背景色覆盖原有鼠标图案
					// 鼠标坐标加上偏移量
					mx += mdec.x;
					my += mdec.y;
					
					// 超边界处理
					if (mx < 0)
						mx = 0;
					if (my < 0)
						my = 0;
					if (mx > binfo->scrnx - 16)
						mx = binfo->scrnx - 16;
					if (my > binfo->scrny - 16)
						my = binfo->scrny - 16;
					sprintf(s, "(%3d, %3d)", mx, my);
					boxFill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 0, 79, 15);	// 覆盖原有坐标信息
					putFont8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);	// 显示新的坐标信息
					putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);	// 显示新的鼠标图案
				} 
			}
		}
	}
}
