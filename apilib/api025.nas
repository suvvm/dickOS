[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "api025.nas"]				; 源文件名信息

		GLOBAL	_apiFileRead
		
[SECTION .text]

_apiFileRead:						; int apiFileRead(char *buf,int maxSize, int fileHandle);
		PUSH	EBX
		MOV		EDX,25				; 功能号25
		MOV		EAX,[ESP+16]		; fileHandle
		MOV		ECX,[ESP+12]		; maxSize
		MOV		EBX,[ESP+8]			; buf
		INT		0x40
		POP		EBX
		RET
