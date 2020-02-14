[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "api020.nas"]				; 源文件名信息

		GLOBAL	_apiBeep
		
[SECTION .text]

_apiBeep:						; void apiBeep(int tone);
		MOV		EDX,20			; 功能号20
		MOV		EAX,[ESP+4]		; tone
		INT		0x40
		RET
