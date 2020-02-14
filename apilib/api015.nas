[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "api015.nas"]				; 源文件名信息

		GLOBAL	_apiGetKey
		
[SECTION .text]

_apiGetKey:						; int apiGetKey(int mode);
		MOV		EDX,15			; 功能号15
		MOV		EAX,[ESP+4]		; mode
		INT		0x40
		RET
