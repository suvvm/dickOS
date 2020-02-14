[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "api009.nas"]				; 源文件名信息

		GLOBAL	_apiMalloc
		
[SECTION .text]

_apiMalloc:						; char *apiMalloc(int size);
		PUSH	EBX
		MOV		EDX,9			; 功能号9
		MOV		EBX,[CS:0x0020]
		MOV		ECX,[ESP+8]		; size
		INT		0x40
		POP		EBX
		RET
