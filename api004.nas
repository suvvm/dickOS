[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "api004.nas"]				; 源文件名信息

		GLOBAL	_apiEnd
		
[SECTION .text]

_apiEnd:						; void apiEnd()
		MOV		EDX,4
		INT		0x40
