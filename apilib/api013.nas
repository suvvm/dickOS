[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "api013.nas"]				; 源文件名信息

		GLOBAL	_apiLineWin
		
[SECTION .text]

_apiLineWin:					; void apiLineWin(int sheet, int startX, int startY, int endX, int endY, int col);
		PUSH	EDI
		PUSH	ESI
		PUSH	EBP
		PUSH	EBX
		MOV		EDX,13			; 功能号13
		MOV		EBX,[ESP+20]	; sheet
		MOV		EAX,[ESP+24]	; startX
		MOV		ECX,[ESP+28]	; startY
		MOV		ESI,[ESP+32]	; endX
		MOV		EDI,[ESP+36]	; endY
		MOV		EBP,[ESP+40]	; col
		INT		0x40
		POP		EBX
		POP		EBP
		POP		ESI
		POP		EDI
		RET
