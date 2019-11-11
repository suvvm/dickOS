/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 1.0.13
* @Date: 2019-11-11
* @Description: ����������Ҫʹ�õĹ��ܺ���
********************************************************************************/
#include "bootpack.h"
#include "desctab.c"
#include "graphic.c"
#include "queue.h"
#include "mouse.c"


/*******************************************************
*
* Function name:memtestSub
* Description: ����start��end��ַ��Χ���ܹ�ʹ�õ��ڴ��ĩβ��ַ
* Parameter:
*	@start	��Ҫ�������ʼ��ַ
*	@end	��Ҫ�����ĩβ��ַ
* Return:
*	�����ܹ�ʹ�õ�ĩβ��ַ
*
**********************************************************/
unsigned int memtestSub(unsigned int start, unsigned int end) {
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
	
	i = memtestSub(start, end);
	
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
	
	initGdtit();	// ��ʼ��GDT IDT
	init_pic();	// ��ʼ���ɱ���жϿ�����
	io_sti();	// ���cpu�жϽ�ֹ
	
	QueueInit(&keybuf, 32, keyb);	//��ʼ�����̻���������
	QueueInit(&mousebuf, 128, mouseb);	// ��ʼ����껺��������
	
	io_out8(PIC0_IMR, 0xf9); // ��PIC IRQ1�����̣���IRQ2����PIC����������(11111001)
	io_out8(PIC1_IMR, 0xef); // ��PIC IRQ12����꣩��������(11101111)
	
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
	
	i = memtest(0x00400000, 0xbfffffff) / (1024 * 1024);
	sprintf(s, "memory %dMB", i);
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
