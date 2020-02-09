[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "helloCFunc.nas"]			; 源文件名信息

		GLOBAL	_apiPutchar, _apiPutstr0, _apiOpenWindow
		GLOBAL	_apiEnd
		
[SECTION .text]

_apiPutchar:					; void apiPutchar(int c)
		MOV		EDX,1			; 功能号1
		MOV		AL,[ESP+4]		; c
		INT		0x40
		RET
		
_apiPutstr0:					; void apiPutstr0(char *s)
		PUSH	EBX
		MOV		EDX,2
		MOV		EBX,[ESP+8]		; s
		INT		0x40
		POP		EBX
		RET
		
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
		
_apiEnd:						; void apiEnd()
		MOV		EDX,4
		INT		0x40
