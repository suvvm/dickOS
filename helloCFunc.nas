[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "helloCFunc.nas"]			; 源文件名信息

		GLOBAL	_apiPutchar
		
[SECTION .text]

_apiPutchar:					; void apiPutchar(int c)
		MOV		EDX,1			; 功能号1
		MOV		AL,[ESP+4]		; c
		INT		0x40
		RET
