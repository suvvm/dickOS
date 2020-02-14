[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "api017.nas"]				; 源文件名信息

		GLOBAL	_apiInitTimer
		
[SECTION .text]

_apiInitTimer:					; void apiInitTimer(int timer, int data);
		PUSH	EBX
		MOV		EDX,17			; 功能号17
		MOV		EBX,[ESP+8]		; timer
		MOV		EAX,[ESP+12]	; data
		INT		0x40
		POP		EBX
		RET
