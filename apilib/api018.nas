[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "api018.nas"]				; 源文件名信息

		GLOBAL	_apiSetTimer
		
[SECTION .text]

_apiSetTimer:					; void apiSetTimer(int timer, int time);
		PUSH	EBX
		MOV		EDX,18			; 功能号18
		MOV		EBX,[ESP+8]		; timer
		MOV		EAX,[ESP+12]	; time
		INT		0x40
		POP		EBX
		RET
