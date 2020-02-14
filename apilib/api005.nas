[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "api005.nas"]				; 源文件名信息

		GLOBAL	_apiOpenWindow
		
[SECTION .text]

_apiOpenWindow:					; int apiOpenWindow(char *buf, int width, int height, int colInvNum, char *title);
		PUSH	EDI
		PUSH	ESI
		PUSH	EBX
		MOV		EDX,5			; 功能号5打印窗口
		MOV		EBX,[ESP+16]	; buf
		MOV		ESI,[ESP+20]	; width
		MOV		EDI,[ESP+24]	; height
		MOV		EAX,[ESP+28]	; colInvNum
		MOV		ECX,[ESP+32]	; title
		INT		0x40
		POP		EBX
		POP		ESI
		POP		EDI
		RET
