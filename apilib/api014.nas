[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "api014.nas"]				; 源文件名信息

		GLOBAL	_apiCloseWin
		
[SECTION .text]

_apiCloseWin:					; void apiCloseWin(int sheet);
		PUSH	EBX
		MOV		EDX,14			; 功能号14
		MOV		EBX,[ESP+8]		; sheet
		INT		0x40
		POP		EBX
		RET
