/********************************************************************************
* @File name: desctab.c
* @Author: suvvm
* @Version: 0.0.4
* @Date: 2020-01-18
* @Description: 包含对GDT，IDT等描述符表的处理
********************************************************************************/
#include "bootpack.h"
#include "interrupt.c"
/*******************************************************
*
* Function name:setSegmdesc
* Description: 设置指定GDT段描述符
* Parameter:
* 	@sd	指向要修改段描述符内存首地址的指针
* 	@limit	段描述符所描述内存段的上限
* 	@base	段描述符所描述内存段基址
* 	@ar	段描述符属性
*
**********************************************************/
void setSegmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar){
	if(limit > 0xfffff){	/*内存段长度超过2^20B(1MB) 将该段定义为4G空间*/
		ar |= 0x8000;	/*G_bit = 1*/
		limit /= 0x1000;	/*单位换算为4KB*/
	}
	sd->limitLow = limit & 0xffff;	/*取limit低16位*/
	sd->baseLow = base & 0xffff;	/*取base低16位*/
	sd->baseMid = (base >> 16) & 0xff;	/*右移16位取低8位*/
	sd->accessRight = ar & 0xff;	/*取属性ar的低8位*/
	sd->limitHigh = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);	/*取limit右移16位后取低4位（就是取limit高4位） | 属性右移8位后取低4位（ar高4位）*/
	sd->baseHigh = (base >> 24) & 0xff;	/*base右移24位后取低8位（base高8位）*/
}

/*******************************************************
*
* Function name:setGatedesc
* Description: 设置指定IDT描述符
* Parameter:
* 	@gd	指向要修改IDT描述符内存首地址的指针
* 	@offset	IDT描述符所在段的偏移地址
* 	@selector	处理程序所在内存段的段符
* 	@ar	IDT描述符属性
*
**********************************************************/
void setGatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar){
	gd->offsetLow = offset & 0xffff;	/*取offset低16位*/
	gd->selector = selector;	/*段选择符*/
	gd->dwCount = (ar >> 8) & 0xff;		/*ar右移8位后取低8位*/
	gd->accessRight = ar & 0xff;	/*取ar低8位*/
	gd->offsetHigh = (offset >> 16) & 0xffff;	/*offset右移16位后取低16位（取offset高16位）*/
}

/*******************************************************
*
* Function name:initGdtit
* Description: 初始化GDT,IDT
*
**********************************************************/
void initGdtit(){
	/*段描述符表GDT地址为 0x270000~0x27ffff*/
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	/*中断描述符表IDT地址为 0x26f800~0x26ffff*/
	struct GATE_DESCRIPTOR *idt = (struct GATE_DESCRIPTOR *) ADR_IDT;
	int i;
	/*将GDT内存段（全8192个）初始化为0*/
	for(i = 0; i <= LIMIT_GDT / 8; i++){
		setSegmdesc(gdt + i, 0, 0, 0);
	}
	/*将段号为1的段上限设为4GB，基址是0，表示的是32位下cpu能管理的全部内存，段属性为0x4092（0100（高4位） 0000（无效位） 10010010（低8位）具体作用见node.txt‬）*/
	setSegmdesc(gdt + 1, 0xffffffff, 0x00000000, AR_DATA32_RW);
	/*将段号为2的段上限设为512KB，基址为0x00280000，为执行bootpack.hrb的段 段属性为（0100（高4位） 0000（无效位） 10011010‬）（低8位）*/
	setSegmdesc(gdt + 2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER);
	/*调用汇编函数向gdtr赋值（将信息加载给寄存器）*/
	loadGdtr(LIMIT_GDT, ADR_GDT);
	/*将IDT描述符（全256个）初始化为0*/
	for(i = 0; i <= LIMIT_IDT / 8; i++){
		setGatedesc(idt + i, 0, 0, 0);
	}
	/*调用汇编函数向idtr赋值（将信息加载给寄存器）*/
	loadIdtr(LIMIT_IDT, ADR_IDT);
	
	//设置IRQ1 IRQ7 IRQ12 的IDT
	setGatedesc(idt + 0x21, (int) asm_interruptHandler21, 2 * 8, AR_INTGATE32);
	setGatedesc(idt + 0x27, (int) asm_interruptHandler27, 2 * 8, AR_INTGATE32);
	setGatedesc(idt + 0x2c, (int) asm_interruptHandler2c, 2 * 8, AR_INTGATE32);
}
