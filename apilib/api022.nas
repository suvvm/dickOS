[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "api022.nas"]				; 源文件名信息

		GLOBAL	_apiFileClose
		
[SECTION .text]

_apiFileClose:						; void apiFileClose(int fileHandle);
		MOV		EDX,22			; 功能号22
		MOV		EAX,[ESP+4]		; fileHandle
		INT		0x40
		RET
