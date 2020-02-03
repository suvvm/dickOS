#ifndef MULTIPOCESS_C
#define MULTIPOCESS_C
/********************************************************************************
* @File name: multiProcess.c
* @Author: suvvm
* @Version: 0.0.5
* @Date: 2020-02-03
* @Description: 定义启动多进程操作相关变量与函数
********************************************************************************/

#include "bootpack.h"
#include "timer.c"

struct PROCESSCTL *processctl;
struct TIMER *mpTimer;	// 进程切换定时器

/*******************************************************
*
* Function name: processNow
* Description: 获取当前正在占用处理机的进程的地址
* Return:
*	返回当前活动进程PCB地址指针
*
*******************************************************/
struct PCB *processNow() {
	struct PROCESSLEVEL *pl = &processctl->level[processctl->nowLv];	// 获取当前活动级
	return pl->processesAcs[pl->now];
}

/*******************************************************
*
* Function name: processAdd
* Description: 向PROSCESSLEVEL中添加指定进程
* Parameter:
*	@process	指定进程PCB地址	struct PCB *	
*
*******************************************************/
void processAdd(struct PCB *process) {
	struct PROCESSLEVEL *pl = &processctl->level[process->level];	// 获取要添加进程的级
	pl->processesAcs[pl->running] = process;	// 将指定进程添加至该级就绪队列队尾
	pl->running++;
	process->status = 2;	// 标记该进程为活动状态
}

/*******************************************************
*
* Function name: processRemove
* Description: 在PROSCESSLEVEL中删除指定进程
* Parameter:
*	@process	指定进程PCB地址	struct PCB *	
*
*******************************************************/
void processRemove(struct PCB *process) {
	int i;
	struct PROCESSLEVEL *pl = &processctl->level[process->level];
	for (i = 0; i < pl->running; i++) {	// 找到指定进程的位置
		if (pl->processesAcs[i] == process) {
			break;
		}
	}
	pl->running--;
	if (i < pl->now) {
		pl->now--;
	}
	if (pl->now >= pl->running) {
		pl->now = 0;
	}
	process->status = 1;	// 进程已分配不工作
	for (; i < pl->running; i++) {
		pl->processesAcs[i] = pl->processesAcs[i + 1];
	}
}

/*******************************************************
*
* Function name: processSwitchSub
* Description: 切换进程时决定切换到哪一层
*
*******************************************************/
void processSwitchSub() {
	int i;
	for (i = 0; i < MAX_PROCESSLEVELS; i++) {
		if (processctl->level[i].running > 0) {	// 找到有活动进程的级
			break;
		}
	}
	processctl->nowLv = i;
	processctl->lvChange = 0;
}

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
	
	for (i = 0; i < MAX_PROCESSLEVELS; i++) {	// 初始化分级反馈队列
		processctl->level[i].running = 0;
		processctl->level[i].now = 0;
	}
	process = processAlloc();	// 获取一个可使用的未运行进程
	process->status = 2;	// 标记进程为活动状态
	process->priority = 2;
	process->level = 0;	// 将最初的进程至于第0级（最高级）
	
	processAdd(process);	// 将该进程添加至对应级
	processSwitchSub();
	
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
*	@level		传入指定进程的层			int
*	@priority	优先级						int
*
*******************************************************/
void processRun(struct PCB *process, int level, int priority) {
	if (level < 0) {	// 不改变进程的级
		level = process->level;
	}
	if (priority > 0) {
		process->priority = priority;
	}
	if (process->status == 2 && process->level != level) {	// 改变正在运行的进程的层
		processRemove(process);	// 先将目标进程在原先的层中移除并停止进程的运行
	}
	if (process->status != 2) {
		process->level = level;	// 改变进程的级
		processAdd(process);	// 将进程添加至对应级并运行
	}
	processctl->lvChange = 1;	//  下次任务切换时检查更换级
}

/*******************************************************
*
* Function name: processSwitch
* Description: 切换至就绪队列下一个进程 至此一个简单的时间片轮转调度就完成了 莫得多级反馈 莫得优先级 也莫得抢占
*
*******************************************************/
void processSwitch() {
	struct PROCESSLEVEL *pl = &processctl->level[processctl->nowLv];
	struct PCB *newProcess, *nowProcess = pl->processesAcs[pl->now];
	pl->now++;
	if (pl->now == pl->running) {	// 修正now
		pl->now = 0;
	}
	if (processctl->lvChange != 0) {
		processSwitchSub();	// 获取当前有就绪进程的最高层并把lvChange置1
		pl = &processctl->level[processctl->nowLv];
	}	
	newProcess = pl->processesAcs[pl->now];
	timerSetTime(mpTimer, newProcess->priority);
	if (newProcess != nowProcess) {
		farJmp(0, newProcess->pid);
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
	struct PCB *nowProcess;
	if (process->status == 2) {	// 要休眠的进程处于活动状态
		nowProcess = processNow();	// 获取当前正在占用处理机的进程
		processRemove(process);	// 将指定进程在其对应级中移除并标记为休眠态
		if (process == nowProcess) {	// 若指定进程便是当前活动进程
			// 进行进程切换
			processSwitchSub();
			nowProcess = processNow();	// 获取新的进程
			farJmp(0, nowProcess->pid);	// 切换至新的进程
		}
	}
}

#endif // MULTIPOCESS_C
