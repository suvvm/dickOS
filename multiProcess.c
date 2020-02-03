#ifndef MULTIPOCESS_C
#define MULTIPOCESS_C
/********************************************************************************
* @File name: multiProcess.c
* @Author: suvvm
* @Version: 0.0.3
* @Date: 2020-02-03
* @Description: 定义启动多进程操作相关变量与函数
********************************************************************************/

#include "bootpack.h"
#include "timer.c"

struct PROCESSCTL *processctl;
struct TIMER *mpTimer;	// 进程切换定时器

/*******************************************************
*
* Function name: processInit
* Description: 进程初始化
* Parameter:
*	@memsegtable	内存段表指针	struct MEMSEGTABLE *	
* Return:
*	返回初始化的进程地址
*
*******************************************************/
struct PCB *processInit(struct MEMSEGTABLE *memsegtable) {
	struct PCB *process;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;	// 段描述符存放GDT内容
	processctl = (struct PROCESSCTL *) memsegAlloc4K(memsegtable, sizeof(struct PROCESSCTL));	// 为进程管理结构体分配内存
	int i;
	for (i = 0; i < MAX_PROCESS; i++) {
		processctl->processes[i].status = 0;	// 所有进程标记为未运行
		processctl->processes[i].pid = (PROCESS_GDT0 + i) * 8;	// 记录进程GDT编号为pid
		setSegmdesc(gdt + PROCESS_GDT0 + i, 103, (int)&processctl->processes[i].tss, AR_TSS32); // 在全局描述符表中定义每个进程状态段 限长103字节
	}
	process = processAlloc();	// 获取一个可使用的未运行进程
	process->status = 2;	// 标记进程为活动状态
	
	processctl->running = 1;	// 当前运行进程数量为1
	processctl->now = 0;
	
	processctl->processesAcs[0] = process;
	
	loadTr(process->pid);	// 通知CPU当前活动进程pid（向TR寄存器赋值）
	
	mpTimer = timerAlloc();
	// 不使用timerInit 因为超时后不向缓冲区中写入数据
	timerSetTime(mpTimer, 2);	// 0.02秒超时一次
	return process;
}

/*******************************************************
*
* Function name: processAlloc
* Description: 进程分配	
* Return:
*	如果有可分配的进程返回分配的进程的地址，否则返回0
*
*******************************************************/
struct PCB *processAlloc() {
	struct PCB *process;
	int i;
	for (i = 0; i < MAX_PROCESS; i++) {
		if (processctl->processes[i].status == 0) {	// 找到未活动的进程
			process = &processctl->processes[i];
			process->status = 1;	// 已分配不工作
			process->tss.eflags = 0x00000202;	// eflags IF位置1
			process->tss.eax = 0;
			process->tss.ecx = 0;
			process->tss.edx = 0;
			process->tss.ebx = 0;
			
			process->tss.ebp = 0;
			process->tss.esi = 0;
			process->tss.edi = 0;
			
			process->tss.es = 0;
			process->tss.ds = 0;
			process->tss.fs = 0;
			process->tss.gs = 0;
			
			process->tss.ldtr = 0;
			process->tss.iomap = 0x40000000;
			return process;
		}
	}
	return 0;
}

/*******************************************************
*
* Function name: processRun
* Description: 进程添加至就绪队列末尾
* Parameter:
*	@process	想要运行的进程控制块指针	struct PCB *	
*
*******************************************************/
void processRun(struct PCB *process) {
	process->status = 2;	// 进程运行标志
	processctl->processesAcs[processctl->running] = process;	// 添加至就绪队列末尾
	processctl->running++;	// 运行进程加一
}

/*******************************************************
*
* Function name: processSwitch
* Description: 切换至就绪队列下一个进程 至此一个简单的时间片轮转调度就完成了 莫得多级反馈 莫得优先级 也莫得抢占
*
*******************************************************/
void processSwitch() {
	timerSetTime(mpTimer, 2);
	if (processctl->running >= 2) {	// 只有一个进程取需切换
		processctl->now++;
		processctl->now %= processctl->running;
		farJmp(0, processctl->processesAcs[processctl->now]->pid);
	}
}

/*******************************************************
*
* Function name: processSleep
* Description: 进程休眠
* Parameter:
*	@process	想要休眠的进程控制块指针	struct PCB *	
*
*******************************************************/
void processSleep(struct PCB *process) {
	int i;
	char tp = 0;	// 标志是否需要切换进程
	if (process->status == 2) {	// 进程处于运行态
		if (process == processctl->processesAcs[processctl->now]) {	// 让自己休眠需要切换进程
			tp = 1;
		}
		
		for (i = 0; i < processctl->running; i++) {
			if (processctl->processesAcs[i] == process) {	// 在就绪队列里找到对应进程
				break;
			}
		}
		processctl->running--;
		if (i < processctl->now) {	// 在当前进程前
			processctl->now--;
		}
		for (; i < processctl->running; i++) {
			processctl->processesAcs[i] = processctl->processesAcs[i + 1];	// 后方进程前移
		}
		process->status = 1;	// 已分配不工作
		if (tp != 0) {	// 需要切换进程
			if (processctl->now >= processctl->running) {	// 修正now值
				processctl->now = 0;
			}
			farJmp(0, processctl->processesAcs[processctl->now]->pid);
		}
	}
}

#endif // MULTIPOCESS_C
