[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "api021.nas"]				; 源文件名信息

		GLOBAL	_apiFileOpen
		
[SECTION .text]

_apiFileOpen:						; int apiFileOpen(char *fileName);
		PUSH 	EBX
		MOV		EDX,21			; 功能号21
		MOV		EBX,[ESP+8]		; fileName
		INT		0x40
		POP		EBX
		RET
