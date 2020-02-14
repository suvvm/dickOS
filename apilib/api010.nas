[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "api010.nas"]				; 源文件名信息

		GLOBAL	_apiFree
		
[SECTION .text]

_apiFree:						; void apiFree(char *addr, int size);
		PUSH	EBX
		MOV		EDX,10			; 功能号10
		MOV		EBX,[CS:0x0020]
		MOV		EAX,[ESP+8]		; addr
		MOV		ECX,[ESP+12]	; size
		INT		0x40
		POP		EBX
		RET
