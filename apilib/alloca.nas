[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "alloca.nas"]				; 源文件名信息

		GLOBAL	__alloca
		
[SECTION .text]

__alloca:						; 供C语言程序调用的 alloca 从栈中分配EAX个字节的内存空间
		ADD		EAX,-4
		SUB		ESP,EAX
		JMP		DWORD [ESP+EAX]
