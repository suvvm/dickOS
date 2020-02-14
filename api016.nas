[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "api016.nas"]				; 源文件名信息

		GLOBAL	_apiAllocTimer
		
[SECTION .text]

_apiAllocTimer:					; int apiAllocTimer();
		MOV		EDX,16			; 功能号16
		INT		0x40
		RET
