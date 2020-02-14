[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "api011.nas"]				; 源文件名信息

		GLOBAL	_apiPoint
		
[SECTION .text]

_apiPoint:						; void apiPoint(int sheet, int x, int y, int col);
		PUSH	EDI
		PUSH	ESI
		PUSH	EBX
		MOV		EDX,11			; 功能号11
		MOV		EBX,[ESP+16]	; sheet图层句柄
		MOV		ESI,[ESP+20]	; x
		MOV		EDI,[ESP+24]	; y
		MOV		EAX,[ESP+28]	; col
		INT		0x40
		POP		EBX
		POP		ESI
		POP		EDI
		RET
