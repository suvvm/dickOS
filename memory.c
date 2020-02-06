#ifndef MEMORY_C
#define MEMORY_C
/********************************************************************************
* @File name: memory.c
* @Author: suvvm
* @Version: 0.0.3
* @Date: 2020-02-06
* @Description: 实现内存相关函数
********************************************************************************/
#include "bootpack.h"

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
	
	i = memtest_sub(start, end);
	
	if(flag486 != 0) {	// 386以上
		cr0 = loadCr0();	// 读取当前cr0寄存器的值
		cr0 &= ~CR0_CACHE_DISABLE;	// 允许缓存	
		storeCr0(cr0);	// 写入cr0寄存器
	}
	
	return i;
}

/*******************************************************
*
* Function name: memsegTotal
* Description: 检查空闲内存大小总和
* Parameter:
*	@memsegtable 段表指针	struct MEMSEGTABLE *	
* Return:
*	返回空闲内存的大小 unsigned int
*
**********************************************************/
unsigned int memsegTotal(struct MEMSEGTABLE *memsegtable) {
	unsigned i, total = 0;
	for(i = 0; i < memsegtable->frees; i++){	// c99以下不允许在for循环内声明变量
		total += memsegtable->free[i].size;
	}
	return total;
}

/*******************************************************
*
* Function name: memsegInit
* Description: 初始化段表信息
* Parameter:
*	@memsegtable 段表指针	struct MEMSEGTABLE *	
*
**********************************************************/
void memsegInit(struct MEMSEGTABLE *memsegtable) {
	memsegtable->frees = 0;
	memsegtable->maxfrees = 0;
	memsegtable->lostsize = 0;
	memsegtable->lostcnt = 0;
}

/*******************************************************
*
* Function name: memsegAlloc
* Description: 内存分配（首次匹配）
* Parameter:
*	@memsegtable	段表指针		struct MEMSEGTABLE *
*	@size			要分配的大小	unsigned int
* Return:
*	成功则返回分配的首地址，失败则返回0	unsigned int
*
**********************************************************/
unsigned int memsegAlloc(struct MEMSEGTABLE *memsegtable, unsigned int size) {
	unsigned int i, alloc;
	for(i = 0; i < memsegtable->frees; i++) {	// c99以下不允许在for循环内声明变量
		if(memsegtable->free[i].size >= size) {	// 找到可分配的内存段
			alloc = memsegtable->free[i].addr;
			memsegtable->free[i].addr += size;	
			memsegtable->free[i].size -= size;
			if(memsegtable->free[i].size == 0) {	// 若当前空闲分段偏移为0证明与下一段重合，将其合并
				memsegtable->frees--;
				for(; i < memsegtable->frees; i++) {
					memsegtable->free[i] = memsegtable->free[i+1];
				}
			}
			return alloc;
		}
	}
	return 0;
}


/*******************************************************
*
* Function name: memsegFree
* Description: 内存释放
* Parameter:
*	@memsegtable	段表指针				struct MEMSEGTABLE *
*	@addr			要释放内存段的首地址	unsigned int
*	@size			要释放的偏移			unsigned int
* Return:
*	释放成功返回0，释放失败返回-1			int
*
**********************************************************/
int memsegFree(struct MEMSEGTABLE *memsegtable, unsigned int addr, unsigned int size) {
	int i, j;
	for(i = 0; i < memsegtable->frees; i++) {	// 找到第一个大于addr的空闲地址
		if(memsegtable->free[i].addr > addr)
			break;
	}
	// 当前状态 free[i-1].addr < addr < free[i].addr
	
	if(i > 0 && memsegtable->free[i-1].addr + memsegtable->free[i-1].size == addr) {	// addr前方有连续的空闲空间
		memsegtable->free[i-1].size += size;	// 将其与当前欲释放空间合并
		if(i < memsegtable->frees && addr + size == memsegtable->free[i].addr) {	// addr 后方有连续的空闲空间
			memsegtable->free[i-1].size += memsegtable->free[i].size;
			memsegtable->frees--;
			for(; i < memsegtable->frees; i++) {
				memsegtable->free[i] = memsegtable->free[i+1];
			}
		}	
		return 0;
	}
	if(i < memsegtable->frees && addr + size == memsegtable->free[i].addr) {	// addr前方没有空闲空间后方却有空闲空间
		memsegtable->free[i].addr = addr;
		memsegtable->free[i].size += size;
		return 0;
	}
	
	if(memsegtable->frees < MEMSEG_MAX) {	// addr前后都没有空闲空间
		for(j = memsegtable->frees; j > i; j--) {
			memsegtable->free[j] = memsegtable->free[j - 1];
		}
		memsegtable->frees++;
		if(memsegtable->maxfrees < memsegtable->frees) {
			memsegtable->maxfrees = memsegtable->frees;	// 更新最大值
		}
		memsegtable->free[i].addr = addr;
		memsegtable->free[i].size = size;
		return 0;
	}
	
	// 内存段超上限
	memsegtable->lostcnt++;
	memsegtable->lostsize += size;
	return -1;
}


/*******************************************************
*
* Function name: memtest_sub
* Description: 调查start到end地址范围内能够使用的内存的末尾地址(由于编译器缘由，在fun.nas中重写为汇编函数)
* Parameter:
*	@start	需要调查的起始地址
*	@end	需要调查的末尾地址
* Return:
*	返回能够使用的末尾地址
*
**********************************************************/
/*
unsigned int memtest_sub(unsigned int start, unsigned int end) {
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
*/

/*******************************************************
*
* Function name: memsegAlloc4K
* Description: 以4KB为单位进行内存分配，用于减少内存碎片
* Parameter:
* 	@memsegtable	段表指针			struct MEMSEGTABLE *
* 	@size			要分配的大小		unsigned int
* Return:
*	成功则返回分配的首地址，失败则返回0	unsigned int
*
**********************************************************/
unsigned int memsegAlloc4K(struct MEMSEGTABLE *memsegtable, unsigned int size) {
	unsigned int alloc;
	size = (size + 0xfff) & 0xfffff000;	// 向上舍入
	alloc = memsegAlloc(memsegtable, size);
	return alloc;
}

/*******************************************************
*
* Function name: memsegFree4K
* Description: 以4KB为单位进行内存释放
* Parameter:
* 	@memsegtable	段表指针		struct MEMSEGTABLE *
* 	@size			要分配的大小	unsigned int
* Return:
*	释释放成功返回0，释放失败返回-1 int
*
**********************************************************/
int memsegFree4K(struct MEMSEGTABLE *memsegtable, unsigned int addr, unsigned int size) {
	int ans;
	size = (size + 0xfff) & 0xfffff000;	// 向上舍入
	ans = memsegFree(memsegtable, addr, size);
	return ans;
}

#endif	//MEMORY_C
