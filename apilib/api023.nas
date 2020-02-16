[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "api023.nas"]				; 源文件名信息

		GLOBAL	_apiFileSeek
		
[SECTION .text]

_apiFileSeek:						; void apiFileSeek(int fileHandle, int offset, int mode);
		PUSH	EBX
		MOV		EDX,23				; 功能号23
		MOV		EAX,[ESP+8]			; fileHandle
		MOV		ECX,[ESP+16]		; mode
		MOV		EBX,[ESP+12]		; offset
		INT		0x40
		POP		EBX
		RET
