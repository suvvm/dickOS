[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "api002.nas"]				; 源文件名信息

		GLOBAL	_apiPutstr0
		
[SECTION .text]

_apiPutstr0:					; void apiPutstr0(char *s)
		PUSH	EBX
		MOV		EDX,2
		MOV		EBX,[ESP+8]		; s
		INT		0x40
		POP		EBX
		RET
