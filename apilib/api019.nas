[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "api019.nas"]				; 源文件名信息

		GLOBAL	_apiFreeTimer
		
[SECTION .text]

_apiFreeTimer:					; void apiFreeTimer(int timer);
		PUSH	EBX
		MOV		EDX,19			; 功能号19
		MOV		EBX,[ESP+8]		; timer
		INT		0x40
		POP		EBX
		RET
