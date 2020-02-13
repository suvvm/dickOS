/********************************************************************************
* @File name: bootpack.c
* @Author: suvvm
* @Version: 0.6.7
* @Date: 2020-02-13
* @Description: ����������Ҫʹ�õĹ��ܺ���
********************************************************************************/
#include "bootpack.h"
#include "desctab.c"
#include "graphic.c"
#include "queue.h"
#include "mouse.c"
#include "keyboard.c"
#include "memory.c"
#include "sheet.c"
#include "timer.c"
#include "window.c"
#include "console.c"
#include "file.c"

/*******************************************************
*
* Function name: keyWinOff
* Description: ʹ��ǰ���ڻ�ȡ�������ݵĴ���ֹͣ��ȡ��������
* Parameter:
*	@keyWin	��ǰ���ڽ��ռ������ݵĴ���ָ��	struct SHEET *
*
**********************************************************/
void keyWinOff(struct SHEET *keyWin) {
	changeWinTitle(keyWin, 0);	// �ı�keyWin���ڱ�����Ϊ�ǻ��ɫ	
	if ((keyWin->status & 0x20) != 0) {	// Ŀ�괰���й��
		QueuePush(&keyWin->process->queue, 3);	// ��Ҫֹͣ���ռ�������Ĵ��ڽ��̻��������͹��������Ϣ
	}
}

/*******************************************************
*
* Function name: keyWinOn
* Description: ʹָ�����ڿ�ʼ���ռ�������
* Parameter:
*	@keyWin	ָ���Ĵ��ڴ���ָ��	struct SHEET *
*
**********************************************************/
void keyWinOn(struct SHEET *keyWin) {
	changeWinTitle(keyWin, 1);	// �ı�keyWin���ڱ�����Ϊ���ɫ
	if ((keyWin->status & 0x20) != 0) {	// Ŀ�괰���й��
		QueuePush(&keyWin->process->queue, 2);	// ��Ҫ���ռ�������Ĵ��ڽ��̻��������͹����ʾ��Ϣ
	}
}

/*******************************************************
*
* Function name:Main
* Description: ������
*
**********************************************************/
void Main(){
	struct BOOTINFO *binfo;
	char s[40];
	int	buf[128], keyCmdBuf[32];	// s����Ҫ����ı�����Ϣ bufΪ�ܻ�����
	int i, x, y, mx, my, bufval, mmx = -1, mmy = -1, mmx2 = 0, newMy = -1, newMx = 0, newWx = 0x7fffffff, newWy = 0; // ���x��λ�� ���y��λ�� ���x��λ�� �����ɫ �ƶ�ģʽx���� �ƶ�ģʽy���� �ƶ�ģʽͼ��x����
	int keyShift = 0, keyLeds, keyCmdWait = -1;	// shift���±�ʶ ���̶�Ӧ������״̬
	struct MouseDec mdec;	// ���������Ϣ
	unsigned int memtotal;
	struct MEMSEGTABLE *memsegtable = (struct MEMSEGTABLE *) MEMSEG_ADDR;	// �ڴ�α�ָ��
	struct SHTCTL *shtctl;	// ͼ����ƿ�ָ��
	struct SHEET *sheetBack, *sheetMouse, *sheet = 0, *keyWin;	// ����ͼ�� ���ͼ�� ����ͼ�� ����̨ͼ�� ������ͼ��ָ�� ��ǰ��������ģʽ�Ĵ���ָ��
	unsigned char *bufBack, bufMouse[256];	// ����ͼ�񻺳��� ���ͼ�񻺳��� ����ͼ�񻺳��� ����̨ͼ�񻺳���
	struct QUEUE queue, keyCmd;	// �ܻ����� �洢������̿��Ƶ�·���͵����ݵĻ�����
	struct PCB *processA, *process;
	
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;	// ��ȡ������Ϣ
	keyLeds = (binfo->leds >> 4) & 7;	// ��ȡ���̸�������״̬ binfo->leds��4~6λ
	
	initGdtit();	// ��ʼ��GDT IDT
	init_pic();	// ��ʼ���ɱ���жϿ�����
	io_sti();	// ���cpu�жϽ�ֹ
		
	QueueInit(&queue, 128, buf, 0);	// ��ʼ������������
	
	initPit();	// ��ʼ����ʱ��
	initKeyboard(&queue, 256);	// ��ʼ�����̿��Ƶ�·
	enableMouse(&queue, 512, &mdec);	// �������
	
	io_out8(PIC0_IMR, 0xf8); // ��PIC IRQ0(��ʱ��) IRQ1�����̣���IRQ2����PIC����������(11111000)
	io_out8(PIC1_IMR, 0xef); // ��PIC IRQ12����꣩��������(11101111)
	QueueInit(&keyCmd, 32, keyCmdBuf, 0);
	
	memtotal = memtest(0x00400000, 0xbfffffff);	// ��ȡ�ڴ��ܺ�
	memsegInit(memsegtable);
	memsegFree(memsegtable, 0x00001000, 0x0009e000);
	memsegFree(memsegtable, 0x00400000, memtotal - 0x00400000);
	
	init_palette();	// ��ʼ��16ɫ��ɫ��
	
	shtctl = shtctlInit(memsegtable, binfo->vram, binfo->scrnx, binfo->scrny);
	
	processA = processInit(memsegtable);	// ����̳�ʼ��
	queue.process = processA;
	processRun(processA, 1, 2);	// ����A�������̣�����level 1 ˢ���ʣ����ȼ���0.02s
	
	*((int *) 0x0fe4) = (int) shtctl;
	*((int *) 0x0fec) = (int) &queue;
	
	sheetBack = sheetAlloc(shtctl);
	bufBack = (unsigned char *) memsegAlloc4K(memsegtable, binfo->scrnx * binfo->scrny);	// ��4KBΪ��λΪ���������ڴ�
	sheetSetbuf(sheetBack, bufBack, binfo->scrnx, binfo->scrny, -1);	// ���ñ���ͼ�㻺��������������Ҫ͸��ɫ��Ϊ-1
	init_GUI(bufBack, binfo->scrnx, binfo->scrny);	// ��ʼ��GUI��bufBack
	
	keyWin = openConsole(shtctl, memtotal);
	
	sheetMouse = sheetAlloc(shtctl);
	sheetSetbuf(sheetMouse, bufMouse, 16, 16, 99);	// �������ͼ�㻺������͸��ɫ
	initMouseCursor8(bufMouse, 99);	// ��ʼ�������bufMouse
	// ��ʼ�������Ϊ��Ļ����
	mx = (binfo->scrnx - 16) / 2;	// ���x��λ��
	my = (binfo->scrny - 28 - 16) / 2;	// ���y��λ��
	
	sheetSlide(sheetBack, 0, 0);	// ����ͼ����ʼ����(0,0)
	sheetSlide(keyWin, 168, 64);	// ����̨���̴�����ʼ����(168, 56)
	// sheetSlide(sheetCons[1], 8, 2);	
	// ����̨���̴�����ʼ����(168, 56)
	sheetSlide(sheetMouse, mx, my);	// �����ͼ�㻬������Ӧλ��
	
	sheetUpdown(sheetBack, 0);	// ������ͼ����������0
	// sheetUpdown(sheetCons[1], 1);	
	// ������̨���̴���ͼ����������1
	sheetUpdown(keyWin, 1);	// ������̨���̴���ͼ����������1
	sheetUpdown(sheetMouse, 2);	// �����ͼ����������5
	
	putFont8AscSheet(sheetBack, 0, 32, COL8_FFFFFF, COL8_008484,  "Welcome to DickOS", 17);	// ��DickOSд�뱳����
	sprintf(s, "(%3d, %3d)", mx, my);	// �����λ�ô���s
	putFont8AscSheet(sheetBack, 0, 0, COL8_FFFFFF, COL8_008484,  s, 10);	// ��sд�뱳����
	
	keyWinOn(keyWin);
	
	// Ϊ�˱���ͼ��̵�ǰ״̬��ͻ����һ��ʼ�Ƚ�������ָʾ��״̬
	QueuePush(&keyCmd, KEYCMD_LED);	// 0xed
	QueuePush(&keyCmd, keyLeds);
	for(;;){
		if (QueueSize(&keyCmd) > 0 && keyCmdWait < 0) {	// �������Ҫ����̿��������͵����ݾͷ�����
			keyCmdWait = QueuePop(&keyCmd);
			waitKeyboardControllerReady();
			io_out8(PORT_KEYDAT, keyCmdWait);	// ����̿��Ƶ�·����
		}
		io_cli();	// ���ж�
		if(QueueSize(&queue) == 0) {	// ֻ�л�����û������ʱ���ܿ����ж�
			if (newMx >= 0) {
				io_sti();
				sheetSlide(sheetMouse, newMx, newMy);
				newMx = -1;
			} else if (newWx != 0x7fffffff) {
				io_sti();
				sheetSlide(sheet, newWx, newWy);
				newWx = 0x7fffffff;
			} else {
				processSleep(processA);	
				// ����A���� ��A����������Զ�������������B
				io_sti();	// ���ж�
			}
		} else {
			bufval = QueuePop(&queue);	// ȡ�����������ж�������
			io_sti();	// ���ж�
			if (keyWin != 0 && keyWin->status == 0) {	// ���봰�ڱ��ر�
				if (shtctl->top == 1) {	// ��ǰ�޴���
					keyWin = 0;
				} else {
					keyWin = shtctl->sheetsAcs[shtctl->top - 1];	// ���봰����Ϊ��ǰ��㴰��
					keyWinOn(keyWin);
				}
			}
			if (256 <= bufval && bufval <= 511) {	// �����ж�����
				sprintf(s, "%02X", bufval - 256);
				putFont8AscSheet(sheetBack, 0, 16, COL8_FFFFFF, COL8_008484, s, 2);	// �������ж���Ϣ��ӡ��������
				if (bufval < 256 + 0x80) {	// ���¼���
					if (keyShift == 0) {	// ��δ����shift
						s[0] = keyboardTable0[bufval - 256];
					} else {	// shift���ڰ���
						s[0] = keyboardTable1[bufval - 256];
					}
				} else {
					s[0] = 0;
				}
				
				if ('A' <= s[0] && s[0] <= 'Z') {	// s[0]ΪӢ����ĸ
					if (((keyLeds & 4) == 0 && keyShift == 0) || ((keyLeds & 4) != 0 && keyShift != 0)) {	// ��Ҫת��ΪСд
						s[0] += 0x20;
					}
				}
				
				if (s[0] != 0 && keyWin != 0) {	// ��ͨ�ַ� �س� �˸�
					QueuePush(&keyWin->process->queue, s[0] + 256);
				}
				if (bufval == 256 + 0x0e && keyWin != 0) {
					QueuePush(&keyWin->process->queue, 8 + 256);
				}
				if (bufval == 256 + 0x1c && keyWin != 0) {
					QueuePush(&keyWin->process->queue, 10 + 256);
				}
				if (bufval == 256 + 0x0f && keyWin != 0) {	// ����tab��
					
					keyWinOff(keyWin);
					i = keyWin->index - 1;	// �ҵ���һ��ͼ��������
					
					if (i == 0) {	// �����һ��Ϊ������
						i = shtctl->top - 1;	// ����һ��������Ϊ�������߲�
					}
					keyWin = shtctl->sheetsAcs[i];	// ��ȡ��ǰ���ռ������ݵ�ͼ��
					keyWinOn(keyWin);
				}
				if (bufval == 256 + 0x2a) {	// ��shift����
					keyShift |= 1;
				}
				if (bufval == 256 + 0x36) {	// ��shift����
					keyShift |= 2;
				}
				if (bufval == 256 + 0xaa) {	// ��shifţ��
					keyShift &= ~1;
				}
				if (bufval == 256 + 0xb6) {	// ��shifţ��
					keyShift &= ~2;
				}
				if (bufval == 256 + 0x3a) {	// CapsLock
					keyLeds ^= 4;	// �ı�keyLeds��Ӧλ
					QueuePush(&keyCmd, KEYCMD_LED);
					QueuePush(&keyCmd, keyLeds);
					// ��0xed keyLeds���뻺�����ȴ�����̿��Ƶ�·����
				}
				if (bufval == 256 + 0x45) {	// NumLock
					keyLeds ^= 2;	// �ı�keyLeds��Ӧλ
					QueuePush(&keyCmd, KEYCMD_LED);
					QueuePush(&keyCmd, keyLeds);
					// ��0xed keyLeds���뻺�����ȴ�����̿��Ƶ�·����
				}
				if (bufval == 256 + 0x46) {	// ScrollLock
					keyLeds ^= 1;	// �ı�keyLeds��Ӧλ
					QueuePush(&keyCmd, KEYCMD_LED);
					QueuePush(&keyCmd, keyLeds);
					// ��0xed keyLeds���뻺�����ȴ�����̿��Ƶ�·����
				}
				if (bufval == 256 + 0x3b && keyShift != 0 && keyWin != 0) {	// shift + F1
					process = keyWin->process;
					if (process != 0 && process->tss.ss0 != 0) {
						consolePutstr0(process->console, "\nBreak(key) :\n");
						io_cli();	// ���ж� �����ڸı�Ĵ���ֵ��ʱ���л���������
						process->tss.eax = (int) &(process->tss.esp0);
						process->tss.eip = (int) asm_endApp;
						io_sti();	// ���ж�
					}					
				}
				if (bufval == 256 + 0x3c && keyShift != 0) {	// shift + F2 �����µĿ���̨
					if (keyWin != 0) {
						keyWinOff(keyWin);	// ��ǰ����ֹͣ���ռ�������
					}
					keyWin = openConsole(shtctl, memtotal);
					sheetSlide(keyWin, 32, 4);
					sheetUpdown(keyWin, shtctl->top);
					keyWinOn(keyWin);
				}
				if (bufval == 256 + 0xfa) {	// ���̿��Ƶ�·���سɹ�����
					keyCmdWait = -1; // �ȴ�������һ�η���
				}
				if (bufval == 256 + 0xfe) {	// ���̿��Ƶ�·����ʧ������
					waitKeyboardControllerReady();	// �ȴ����̿��Ƶ�·����
					io_out8(PORT_KEYDAT, keyCmdWait);	// ��������̿��Ƶ�·����8λ����
				}
				if (bufval == 256 + 0x57 && shtctl->top > 2) {	// ����F11 �����ϲ�ͼ����������2 ��ȥ���ͱ�������������2��ͼ��
					sheetUpdown(shtctl->sheetsAcs[1], shtctl->top - 1);	// ����ȥ������֮�⴦�����²��ͼ������������߲�
				}
			} else if (512 <= bufval && bufval <= 767) {	// ����ж�����
				if (mouseDecode(&mdec, bufval - 512) != 0) {	// ���һ�������ֽ����ݵĽ��ջ��߳���δ֪���
					// ����������ƫ����
					mx += mdec.x;
					my += mdec.y;
					
					// ���߽紦��
					if (mx < 0)
						mx = 0;
					if (my < 0)
						my = 0;
					if (mx > binfo->scrnx - 1)
						mx = binfo->scrnx - 1;
					if (my > binfo->scrny - 1)
						my = binfo->scrny - 1;
					
					newMx = mx;
					newMy = my;
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					if ((mdec.btn & 0x01) != 0) {	// �������
						s[1] = 'L';
						// sheetSlide(sheetWin, mx - 80, my - 8);
						if (mmx < 0) {	// mmx = -1 mmy = -1 ʱ�������ƶ�ģʽ
							for (i = shtctl->top - 1; i > 0; i--) {	// ������µ�һ�㿪ʼ���Զ����±���ͼ��
								sheet = shtctl->sheetsAcs[i];
								x = mx - sheet->locationX;
								y = my - sheet->locationY;
								if (0 <= x && x < sheet->width && 0 <= y && y < sheet->height) {	// �����е��������ڸ�ͼ��
									if (sheet->buf[y * sheet->width + x] != sheet->colInvNum) {	// ��ǰ�����е�����Ϊ��ͼ��͸��ɫ
										sheetUpdown(sheet, shtctl->top - 1);	// �������е�ͼ����ֲ�������߲�
										if (sheet != keyWin) {
											keyWinOff(keyWin);
											keyWin = sheet;
											keyWinOn(keyWin);
										}
										if (3 <= x && x < sheet->width - 3 && 3 <= y && y < 21) {	// ��ǰ�����е�Ϊ���ڱ�����
											mmx = mx;	// �ƶ�ģʽ��¼��굱ǰ����
											mmy = my;
											mmx2 = sheet->locationX;
											newWy = sheet->locationY;
										}
										if (sheet->width - 21 <= x && x < sheet->width - 5 && 5 <= y && y < 19) {	// ��ǰ�����е�Ϊ���ڹرհ�ť
											if ((sheet->status & 0x10) != 0) {	// �ô�������ĳһӦ�ó���
												process = sheet->process;
												consolePutstr0(process->console, "\n Break(mouse) :\n");
												io_cli();	// ǿ�ƽ�������ʱ���ж�
												process->tss.eax = (int) &(process->tss.esp0);
												process->tss.eip = (int) asm_endApp;
												io_sti();	// ���ж�	
											}
										}
										break;
									}
								}
							}
						} else {
							// ��������ƶ��ľ���
							x = mx - mmx;
							y = my - mmy;
							newWx = (mmx2 + x + 2) & ~3;
							newWy += y;
							// sheetSlide(sheet, (sheet->locationX + x + 2) & ~3, sheet->locationY + y);	// �ƶ�ͼ��
							// �����ƶ��������
							mmy = my;
						}
					} else {	// û�а������
						mmx = -1;	// �˳��ƶ�ģʽ
						if (newWx != 0x7fffffff) {
							sheetSlide(sheet, newWx, newWy);
							newWx = 0x7fffffff;						}
					}
					if ((mdec.btn & 0x02) != 0) {	// �����Ҽ�
						s[3] = 'R';
					}
					if ((mdec.btn & 0x04) != 0) {	// �м����
						s[2] = 'C';
					}
					putFont8AscSheet(sheetBack, 32, 16, COL8_FFFFFF, COL8_008484, s, 15);	// �ڱ������ӡ��갴����Ϣ
					
					sprintf(s, "(%4d, %4d)", mx, my);
					putFont8AscSheet(sheetBack, 0, 0, COL8_FFFFFF, COL8_008484, s, 12);	// �ڱ������ӡ���������Ϣ
					sheetSlide(sheetMouse, mx, my);
				} 			
			} else if (768 <= bufval && bufval <= 1023) {	// �����йرմ���
				closeConsole(shtctl->sheets + (bufval - 768));
			}
		}
	}
}
