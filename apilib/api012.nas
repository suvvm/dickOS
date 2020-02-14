[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "api012.nas"]				; 源文件名信息

		GLOBAL	_apiRefreshWin
		
[SECTION .text]

_apiRefreshWin:					; void apiRefreshWin(int sheet, int startX, int startY, int endX, int endY);
		PUSH	EDI
		PUSH	ESI
		PUSH	EBX
		MOV		EDX,12			; 功能号12
		MOV		EBX,[ESP+16]	; sheet 图层句柄
		MOV		EAX,[ESP+20]	; startX
		MOV		ECX,[ESP+24]	; startY
		MOV		ESI,[ESP+28]	; endX
		MOV		EDI,[ESP+32]	; endY
		INT		0x40
		POP		EBX
		POP		ESI
		POP		EDI
		RET
