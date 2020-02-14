[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "api008.nas"]				; 源文件名信息

		GLOBAL	_apiInitMalloc
		
[SECTION .text]

_apiInitMalloc:					; void apiInitMalloc();
		PUSH	EBX
		MOV		EDX,8			; 功能号8
		MOV		EBX,[CS:0x0020]	; 在应用程序文件中读取0x0020位置记录的当前malloc可分配内存空间起始地址（memsegtable管理空间起始地址）
		MOV		EAX,EBX
		ADD		EAX,32*1024		; 加上32KB
		MOV		ECX,[CS:0x0000]	; 在应用程序文件中读取0x0000位置读取数据段的大小（memsegtable管理的字节数）
		SUB		ECX,EAX
		INT		0x40
		POP		EBX
		RET
