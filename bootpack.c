/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 1.0.15
* @Date: 2020-01-17
* @Description: ����������Ҫʹ�õĹ��ܺ���
********************************************************************************/
#include "bootpack.h"
#include "desctab.c"
#include "graphic.c"
#include "queue.h"
#include "mouse.c"

/*******************************************************
*
* Function name: memsegInit
* Description: ��ʼ���α���Ϣ
* Parameter:
*	@memsegtable �α�ָ��	struct MEMSEGTABLE *	
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
* Function name: memsegTotal
* Description: �������ڴ��С�ܺ�
* Parameter:
*	@memsegtable �α�ָ��	struct MEMSEGTABLE *	
* Return:
*	���ؿ����ڴ�Ĵ�С unsigned int
*
**********************************************************/
unsigned int memsegTotal(struct MEMSEGTABLE *memsegtable) {
	unsigned i, total = 0;
	for(i = 0; i < memsegtable->frees; i++){	// c99���²�������forѭ������������
		total += memsegtable->free[i].size;
	}
	return total;
}

/*******************************************************
*
* Function name: memsegAlloc
* Description: �ڴ���䣨�״�ƥ�䣩
* Parameter:
*	@memsegtable	�α�ָ��		struct MEMSEGTABLE *
*	@size			Ҫ����Ĵ�С	unsigned int
* Return:
*	�ɹ��򷵻ط�����׵�ַ��ʧ���򷵻�0	unsigned int
*
**********************************************************/
unsigned int memsegAlloc(struct MEMSEGTABLE *memsegtable, unsigned int size) {
	unsigned int i, alloc;
	for(i = 0; i < memsegtable->frees; i++) {	// c99���²�������forѭ������������
		if(memsegtable->free[i].size >= size) {	// �ҵ��ɷ�����ڴ��
			alloc = memsegtable->free[i].addr;
			memsegtable->free[i].addr += size;	
			memsegtable->free[i].size -= size;
			if(memsegtable->free[i].size == 0) {	// ����ǰ���зֶ�ƫ��Ϊ0֤������һ���غϣ�����ϲ�
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
* Description: �ڴ��ͷ�
* Parameter:
*	@memsegtable	�α�ָ��				struct MEMSEGTABLE *
*	@addr			Ҫ�ͷ��ڴ�ε��׵�ַ	unsigned int
*	@size			Ҫ�ͷŵ�ƫ��			unsigned int
* Return:
*	�ͷųɹ�����0���ͷ�ʧ�ܷ���-1
*
**********************************************************/
int memsegFree(struct MEMSEGTABLE *memsegtable, unsigned int addr, unsigned int size) {
	int i, j;
	for(i = 0; i < memsegtable->frees; i++) {	// �ҵ���һ������addr�Ŀ��е�ַ
		if(memsegtable->free[i].addr > addr)
			break;
	}
	// ��ǰ״̬ free[i-1].addr < addr < free[i].addr
	
	if(i > 0 && memsegtable->free[i-1].addr + memsegtable->free[i-1].size == addr) {	// addrǰ���������Ŀ��пռ�
		memsegtable->free[i-1].size += size;	// �����뵱ǰ���ͷſռ�ϲ�
		if(i < memsegtable->frees && addr + size == memsegtable->free[i].addr) {	// addr ���������Ŀ��пռ�
			memsegtable->free[i-1].size += memsegtable->free[i].size;
			memsegtable->frees--;
			for(; i < memsegtable->frees; i++) {
				memsegtable->free[i] = memsegtable->free[i+1];
			}
		}	
		return 0;
	}
	if(i < memsegtable->frees && addr + size == memsegtable->free[i].addr) {	// addrǰ��û�п��пռ��ȴ�п��пռ�
		memsegtable->free[i].addr = addr;
		memsegtable->free[i].size += size;
		return 0;
	}
	
	if(memsegtable->frees < MEMSEG_MAX) {	// addrǰ��û�п��пռ�
		for(j = memsegtable->frees; j > i; j--) {
			memsegtable->free[j] = memsegtable->free[j - 1];
		}
		memsegtable->frees++;
		if(memsegtable->maxfrees < memsegtable->frees) {
			memsegtable->maxfrees = memsegtable->frees;	// �������ֵ
		}
		memsegtable->free[i].addr = addr;
		memsegtable->free[i].size = size;
		return 0;
	}
	
	// �ڴ�γ�����
	memsegtable->lostcnt++;
	memsegtable->lostsize += size;
	return -1;
}

/*******************************************************
*
* Function name: memtest_sub
* Description: ����start��end��ַ��Χ���ܹ�ʹ�õ��ڴ��ĩβ��ַ(���ڱ�����Ե�ɣ���fun.nas����дΪ��ຯ��)
* Parameter:
*	@start	��Ҫ�������ʼ��ַ
*	@end	��Ҫ�����ĩβ��ַ
* Return:
*	�����ܹ�ʹ�õ�ĩβ��ַ
*
**********************************************************/
/*
unsigned int memtest_sub(unsigned int start, unsigned int end) {
	unsigned int i, *p, old, pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;
	for(i = start; i <= end; i += 0x1000) {	// ����ÿ��Ҫ�����ڴ����д����Կ����Ƿ��ܶ�����ȷ��ֵ
		p = (unsigned int *)(i + 0xffc);	// ���4KBĩβ4�ֽ�
		old = *p;	// ��¼��ǰ�޸ĵ�ֵ
		*p = pat0;	// ��д
		*p ^= 0xffffffff;	// ��ȫ1��λ��򣨷�ת�����з�ת��ԭ�������ֱ�Ӷ����Ļ���Щ���ͻ�ֱ�Ӷ���Ҫд������ݣ��𲻵�����Ч��
		if(*p != pat1) {
notMemoey:
			*p = old;
			break;
		}
		*p ^= 0xffffffff;	// �ٴη�ת
		if(*p != pat0) {	// ����ٴη�ת��ֵ�Ƿ�ָ�Ϊ��ֵ
			goto notMemoey;
		}
		*p = old;
	}
	return i;
}
*/
/*******************************************************
*
* Function name: memtest
* Description: ���CPU�Ƿ���486���ϣ�386û��cache��
*	���CPU��486���Ͼͽ�������ΪOFF��֮����þ��崦��������ڴ����ʹ�õ�ĩβ��ַ
* Parameter:
*	@start	��Ҫ�������ʼ��ַ
*	@end	��Ҫ�����ĩβ��ַ
* Return:
*	�����ܹ�ʹ�õ�ĩβ��ַ
**********************************************************/
unsigned int memtest(unsigned int start, unsigned int end) {
	char flag486 = 0;
	unsigned int eflag, cr0, i;
	eflag = io_load_eflags();	// ��ȡ��ǰeflags��ֵ
	eflag |= EFLAGS_AC_BIT;		// �ֶ���eflags��ACλ��Ϊ1
	io_store_eflags(eflag);	// д��eflags
	eflag = io_load_eflags();	//�ٴζ�ȡeflags��ֵ
	if((eflag & EFLAGS_AC_BIT) != 0) {	// �����386����ʹ�ֶ���Ϊ1ACҲ��ָ�Ϊ0
		flag486 = 1;	// ���ACλ��Ϊ����һ����386����
	}
	eflag &= ~EFLAGS_AC_BIT;	// ��eflagsACλ�ָ�Ϊ0
	io_store_eflags(eflag);	// д��eflags
	
	if(flag486 != 0) {	// 386����
		cr0 = loadCr0();	// ��ȡ��ǰcr0�Ĵ�����ֵ
		cr0 |= CR0_CACHE_DISABLE;	// ��ֹ����	
		storeCr0(cr0);	// д��cr0�Ĵ���
	}
	
	i = memtest_sub(start, end);
	
	if(flag486 != 0) {	// 386����
		cr0 = loadCr0();	// ��ȡ��ǰcr0�Ĵ�����ֵ
		cr0 &= ~CR0_CACHE_DISABLE;	// ������	
		storeCr0(cr0);	// д��cr0�Ĵ���
	}
	
	return i;
}

/*******************************************************
*
* Function name:Main
* Description: ������
*
**********************************************************/
void Main(){
	struct BOOTINFO *binfo;
	char mcursor[256], s[40], keyb[32], mouseb[128];	// mcursor�����Ϣ s����Ҫ����ı�����Ϣ
	int mx, my, bufval, i; //���x��λ�� ���y��λ�� Ҫ��ʾ�Ļ�������Ϣ
	struct MouseDec mdec;	// ���������Ϣ
	unsigned int memtotal;
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;
	
	initGdtit();	// ��ʼ��GDT IDT
	init_pic();	// ��ʼ���ɱ���жϿ�����
	io_sti();	// ���cpu�жϽ�ֹ
	
	QueueInit(&keybuf, 32, keyb);	//��ʼ�����̻���������
	QueueInit(&mousebuf, 128, mouseb);	// ��ʼ����껺��������
	
	io_out8(PIC0_IMR, 0xf9); // ��PIC IRQ1�����̣���IRQ2����PIC����������(11111001)
	io_out8(PIC1_IMR, 0xef); // ��PIC IRQ12����꣩��������(11101111)
	
	memtotal = memtest(0x00400000, 0xbfffffff);	// ��ȡ�ڴ��ܺ�
	memsegInit(memsegtable);
	memsegFree(memsegtable, 0x00001000, 0x0009e000);
	memsegFree(memsegtable, 0x00400000, memtotal - 0x00400000);
	
	initKeyboard();
	
	init_palette();	// ��ʼ��16ɫ��ɫ��
	
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;	// ��ȡ������Ϣ
	mx = (binfo->scrnx - 16) / 2;	// ���x��λ��
	my = (binfo->scrny - 28 - 16) / 2;	// ���y��λ��
	init_GUI(binfo->vram, binfo->scrnx, binfo->scrny);	// ��ʼ��GUI����
	
	// ��ӡDICKOS
	putFont8_asc(binfo->vram, binfo->scrnx, 30, 32, COL8_FFFFFF, "DickOS");
	initMouseCursor8(mcursor, COL8_008484);	// ��ʼ�������Ϣ
	// ���������Ϣ��ӡ���
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
	sprintf(s, "(%d, %d)", mx, my);	// �����λ�ô���s
	// ��ӡs
	putFont8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
	
	sprintf(s, "memory %dMB	free : %dKB", memtest(0x00400000, 0xbfffffff) / (1024 * 1024), memsegTotal(memsegtable) / 1024);	// ��ӡ�ڴ���Ϣ
	putFont8_asc(binfo->vram, binfo->scrnx, 0, 64, COL8_FFFFFF, s);
	
	enableMouse(&mdec);	// �������
	//�������������ж������hlt
	for(;;){
		io_cli();
		if(QueueSize(&keybuf) + QueueSize(&mousebuf) == 0) {	// ֻ����������������û������ʱ���ܿ����жϲ�����hltģʽ
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
				if (mouseDecode(&mdec, bufval) != 0) {	//���һ�������ֽ����ݵĽ��ջ��߳���δ֪���
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					if ((mdec.btn & 0x01) != 0) {	// �������
						s[1] = 'L';
					}
					if ((mdec.btn & 0x02) != 0) {	// �����Ҽ�
						s[3] = 'R';
					}
					if ((mdec.btn & 0x04) != 0) {	// �м����
						s[2] = 'C';
					}
					
					boxFill8(binfo->vram, binfo->scrnx, COL8_008484, 32, 16, 32 + 15 * 8 - 1, 31);	// �ñ���ɫ����ԭ�������Ϣ
					putFont8_asc(binfo->vram, binfo->scrnx, 32, 16, COL8_FFFFFF, s);	//��ӡ�����Ϣ
					// ����ƶ�����
					boxFill8(binfo->vram, binfo->scrnx, COL8_008484, mx, my, mx + 15, my + 15);	// �Ա���ɫ����ԭ�����ͼ��
					// ����������ƫ����
					mx += mdec.x;
					my += mdec.y;
					
					// ���߽紦��
					if (mx < 0)
						mx = 0;
					if (my < 0)
						my = 0;
					if (mx > binfo->scrnx - 16)
						mx = binfo->scrnx - 16;
					if (my > binfo->scrny - 16)
						my = binfo->scrny - 16;
					sprintf(s, "(%3d, %3d)", mx, my);
					boxFill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 0, 79, 15);	// ����ԭ��������Ϣ
					putFont8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);	// ��ʾ�µ�������Ϣ
					putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);	// ��ʾ�µ����ͼ��
				} 
			}
		}
	}
}
