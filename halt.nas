; halt
; TAB=4

[FORMAT "WCOFF"]				; 制作目标文件的模式
[BITS 32]						; 制作32位模式

; 制作目标文件的信息

[FILE "halt.nas"]				; 源文件名
		GLOBAL _io_hlt			; 程序中包含的函数名

; 实际的函数

[SECTION .text]					; 目标文件中写了这些后再写程序

_io_hlt:						; void io_hlt(void)
		HLT
		RET
		
; 用汇编语言编写函数io_hlt
