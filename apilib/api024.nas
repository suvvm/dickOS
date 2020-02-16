[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "api024.nas"]				; 源文件名信息

		GLOBAL	_apiFileSize
		
[SECTION .text]

_apiFileSize:						; int apiFileSize(int fileHandle, int mode);
		MOV		EDX,24				; 功能号24
		MOV		EAX,[ESP+4]			; fileHandle
		MOV		ECX,[ESP+8]			; mode
		INT		0x40
		RET
