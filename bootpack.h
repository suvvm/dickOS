/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 0.2.4
* @Date: 2020-01-31
* @Description: 函数结构体声明与宏定义
********************************************************************************/

#ifndef BOOTPACK_H
#define BOOTPACK_H

#include <stdio.h>

/*色号信息*/
#define COL8_000000		0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15

/*描述符信息*/
#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_INTGATE32	0x008e

/*PIC端口信息*/
#define PIC0_ICW1		0x0020	// 写主PIC ICW1的端口地址
#define PIC0_OCW2		0x0020	// 写主PIC OCW2的端口地址
#define PIC0_IMR		0x0021	// 写主PIC 由OCW1写IMR寄存器的端口地址
#define PIC0_ICW2		0x0021	// 写主PIC ICW2的端口地址
#define PIC0_ICW3		0x0021	// 写主PIC ICW3的端口地址
#define PIC0_ICW4		0x0021	// 写主PIC ICW4的端口地址
#define PIC1_ICW1		0x00a0	// 写从PIC ICW1的端口地址
#define PIC1_OCW2		0x00a0	// 写从PIC OCW2的端口地址
#define PIC1_IMR		0x00a1	// 写从PIC 由OCW1写IMR寄存器的端口地址
#define PIC1_ICW2		0x00a1	// 写从PIC ICW2的端口地址
#define PIC1_ICW3		0x00a1	// 写从PIC ICW3的端口地址
#define PIC1_ICW4		0x00a1	// 写从PIC ICW4的端口地址

#define ADR_BOOTINFO	0x00000ff0
#define PORT_KEYDAT		0x0060
#define FLAGS_OVERRUN		0x0001

// 键盘控制电路信息
#define PORT_KEYDAT				0x0060	// 键盘控制电路模式设置端口
#define PORT_KEYSTA				0x0064	// 识别键盘控制电路是否就绪的地址
#define PORT_KEYCMD				0x0064	// 键盘控制电路命令接收端口
#define KEYSTA_SEND_NOTREADY	0x02	// 倒数第二位就绪标识
#define KEYCMD_WRITE_MODE		0x60	// 模式设置指令
#define KBC_MODE				0x47	// 可用鼠标模式
#define KEYCMD_SENDTO_MOUSE		0xd4	// 向键盘控制电路发送0xd4后下一个数据将发送给鼠标
#define MOUSECMD_ENABLE			0xf4	// 激活鼠标的指令

#define EFLAGS_AC_BIT		0x00040000	// 用于判断是否为386（486以上eflags第18位为AC标志位）
#define CR0_CACHE_DISABLE	0x60000000	// 用于开放与禁止缓存

#define MEMSEG_MAX	4090		// 内存分段最大段数 总大小约为32KB
#define MEMSEG_ADDR	0x003c0000	// 内存保留地址（存储分段信息）32KB	

#define MAX_SHEETS	256	// 最大图层数
#define SHEET_USE	1	// 图层使用标记

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040
#define MAX_TIMER	500	// 最大定时器数量
#define TIMER_ALLOC	1	// 定时器运行已配置
#define TIMER_USING	2	// 定时器正在运行

/********************************************************************************
*
* 启动信息，与asmhead.nas中设置一致
* Parameter:
*	@cyls 启动区设置
*	@leds 键盘指示灯LED状态数
*	@vmode 颜色数目信息 颜色的位数
*	@scrnx 分辨率X像素数
*	@scrny 分辨率Y像素数
*	@vram 图像缓冲区开始地址
*
********************************************************************************/
struct BOOTINFO{	
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char *vram;
};

/********************************************************************************
*
* 段描述符存放GDT内容 
* base代表32位段的地址（基址）
* base又分为 low（2字节） mid（1字节） high（1字节）共(2 + 1 + 1) * 8 = 32位
* 32位操作系统段的最大上限位4GB（32位最大数字）
* 但不能直接设为4GB，这样会将32位直接占满，导致没有空间存储段的管理属性信息
* 段的可用上限只有20位，为了解决这个问题inter开发人员在段属性中设置了一个标准位
* Gbit 当Gbit为1时段上限的单位为4KB 4KB * 1MB（20位）= 4GB
* Parameter:
* 	@limitLow 段上限低地址 2字节 16位
*	@baseLow 基址低地址 16位
*	@baseMid 基址中地址 8位
*	@accessRight 段属性低8位（高4位在limitHigh的高4位代表扩展访问权）关于低8位详见note.txt
*	@limitHigh limitHigh 段上限高地址 1字节 8位 由于段上限只有20位，所以在limitHigh高4位也写入段的属性
*	@baseHigh 基址高地址 8位 4位扩展访问权由GD00组成 G为Gbit标志位 D为模式位 1代表32位模式 0代表16位模式（即使D为0也不能调用BOIS）
* 
********************************************************************************/
struct SEGMENT_DESCRIPTOR{
	short limitLow;
	short baseLow;
	char baseMid;
	char accessRight;
	char limitHigh;
	char baseHigh;	
};

/********************************************************************************
*
*门描述符存放IDT内容
*
********************************************************************************/
struct GATE_DESCRIPTOR{
	short offsetLow, selector;
	char dwCount, accessRight;
	short offsetHigh;
};

/********************************************************************************
*
* 定义队列作为各种处理的缓冲区
* buf为队列内数据
* back为队尾，作为下一个写入（入队）位置，front为队首，作为下一个读取（出队）位置
* size为队列大小
* free记录当前可用空间
* 如果缓冲区满时又来了一个数据则将flags标记为1 表明有过溢出
*
********************************************************************************/
struct QUEUE {
	unsigned char *buf;
	int back, front, size, free, flags;
}keybuf, mousebuf;

/********************************************************************************
*
* 将鼠标相关信息提取为一个结构体
* Parameter:
* 	@sd	鼠标保存一条信息的3字节数据
* 	@phase	记录当前在处理那一段数据
* 	@x	鼠标x轴位置
* 	@y	鼠标y轴位置
* 	@btn 鼠标按键状态
*
********************************************************************************/
struct MouseDec {
	unsigned char buf[3], phase;
	int x, y, btn;
};

/********************************************************************************
*
* Memory segmentation information 内存分段信息 记录段首地址与段偏移
* Parameter:
*	@addr 段首地址	unsigned int
*	@size 段偏移	unsigned int
*
********************************************************************************/
struct MEMSEGINFO {
	unsigned int addr, size;
};

/********************************************************************************
*
* Memory segmentation table 段表，用于记录当前可用与分配的分段信息
* Parameter:
*	@frees		当前可用分段数量						int
*	@maxfree	当前最大可用分段数量					int
*	@lostsize	当前释放失败的内存的大小总和			int
*	@lostcnt	当前释放失败的次数						int
*	@table		段表详情，记录着每一段的分段信息		MEMSEGINFO * 
*
********************************************************************************/
struct MEMSEGTABLE {
	int frees, maxfrees, lostsize, lostcnt;
	struct MEMSEGINFO free[MEMSEG_MAX];
};

/********************************************************************************
*
* sheet 图层，用于实现画面叠加处理
* Parameter:
*	@buf		图层上所描画内容的地址					unsigned char *
*	@width		图层的宽度								int
*	@height		图层的高度								int
*	@locationX	图层左上角在x轴上的位置					int
*	@locationY	图层左上角在y轴上的位置					int
*	@colInvNum	透明色号								int
*	@index		图层索引编号							int
*	@status		图层状态								int
*	@shtctl		图层控制块指针							struct SHTCTL *
*
********************************************************************************/
struct SHEET {
	unsigned char *buf;
	int width, height, locationX, locationY, colInvNum, index, status;
	struct SHTCTL *shtctl;
};

/********************************************************************************
*
* sheet control 图层控制，用于管理图层
* Parameter:
*	@vram			对应vram地址							unsigned char *
*	@map			与vram大小相等的内存空间，				unsigned char *
*					用于记录画面上正在显示的点属于哪个图层	
*	@xSize			vram的画面宽度							int
*	@ySize			vram的画面高度							int
*	@top			最上层图层索引							int
*	@sheetsAcs[]	存放按索引升序排列后的所有图层地址		struct SHEET *
*	@sheets[]		存放所有图层							struct SHEET sheets
*
********************************************************************************/
struct SHTCTL {
	unsigned char *vram, *map;
	int xSize, ySize, top;
	struct SHEET *sheetsAcs[MAX_SHEETS];
	struct SHEET sheets[MAX_SHEETS];
};

/********************************************************************************
*
* timer 定时器
* Parameter:
*	@status		记录定时器状态	unsigned int
*	@timeout	超时限制		unsigned int
*	@queue		缓冲区			struct QUEUE *
*	@data		超时信息		unsigned char		
*
********************************************************************************/
struct TIMER {
	unsigned int timeout, status;
	struct QUEUE *queue;
	unsigned char data;
};

/********************************************************************************
*
* timer control 定时器控制块
* Parameter:
*	@count		记录定时器中断发生次数	unsigned int
*	@timer[]	储存定时器				struct TIMER
*
********************************************************************************/
struct TIMERCTL {
	unsigned int count;
	struct TIMER timer[MAX_TIMER];
};

// queue.c 函数声明
void QueueInit(struct QUEUE *fifo, int size, unsigned char *buf);
int QueuePush(struct QUEUE *fifo, unsigned char data);
int QueuePop(struct QUEUE *fifo);
int QueueSize(struct QUEUE *fifo);

// func.nas 函数声明
void io_hlt();	
void io_cli();
void io_sti();
void io_stihlt();
int io_in8(int port);
void io_out8(int port, int data);
int io_load_eflags();
void io_store_eflags(int eflags);
int loadCr0();
void storeCr0(int cr0);
void loadGdtr(int limit, int addr);
void loadIdtr(int limit, int addr);
void asm_interruptHandler20();
void asm_interruptHandler21();
void asm_interruptHandler27();
void asm_interruptHandler2c();
unsigned int memtest_sub(unsigned int start, unsigned int end);

// graphic.c 函数声明
void init_palette();
void set_palette(int start, int end, unsigned char *rgb);
void boxFill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_GUI(char *vram, int xsize, int ysize);
void putFont8(char *vram, int xsize, int x, int y, char c, char *font);
void putFont8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void initMouseCursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize,
int pysize, int px0, int py0, char *buf, int bxsize);
	
// desctab.c 函数声明
void initGdtit();
void setSegmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void setGatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

// interrupt.c 函数声明
void init_pic();
void interruptHandler21(int *esp);
void interruptHandler27(int *esp);
void interruptHandler2c(int *esp);

// keyboard.c 函数声明
void initKeyboard();
void waitKeyboardControllerReady();

// mouse.c 函数声明
void enableMouse();

// memory.c 函数声明
unsigned int memtest(unsigned int start, unsigned int end);
unsigned int memsegTotal(struct MEMSEGTABLE *memsegtable);
void memsegInit(struct MEMSEGTABLE *memsegtable);
unsigned int memsegAlloc(struct MEMSEGTABLE *memsegtable, unsigned int size);
int memsegFree(struct MEMSEGTABLE *memsegtable, unsigned int addr, unsigned int size);
unsigned int memsegAlloc4K(struct MEMSEGTABLE *memsegtable, unsigned int size);
int memsegFree4K(struct MEMSEGTABLE *memsegtable, unsigned int addr, unsigned int size);

// sheet.c 函数声明
struct SHTCTL *shtctlInit(struct MEMSEGTABLE *memsegtable, unsigned char *vram, int xSize, int ySize);
struct SHEET *sheetAlloc(struct SHTCTL *shtclt);
void sheetSetbuf(struct SHEET *sheet, unsigned char *buf, int width, int height, int colInvNum);
void sheetUpdown(struct SHEET *sheet, int index);
void sheetRefresh(struct SHEET *sheet, int startX, int startY, int endX, int endY);
void sheetRefreshSub(struct SHTCTL *shtctl, int startX, int startY, int endX, int endY, int startIndex);
void sheetSlide(struct SHEET *sheet, int locationX, int locationY);
void sheetFree(struct SHEET *sheet);
void sheetRefreshMap(struct SHTCTL *shtctl, int startX, int startY, int endX, int endY, int startIndex);

// timer.c 函数声明
void initPit();
void interruptHandler20(int *esp);
void timerSetTime(struct TIMER *timer, unsigned int timeout);
void timerInit(struct TIMER *timer, struct QUEUE *queue, unsigned char data);
void timerFree(struct TIMER *timer);
struct TIMER *timerAlloc();


#endif // BOOTPACK_H
