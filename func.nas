; halt
; TAB=4

[FORMAT "WCOFF"]				; 制作目标文件的模式
[INSTRSET "i486p"]				
[BITS 32]						; 制作32位模式

; 制作目标文件的信息

[FILE "halt.nas"]				; 源文件名
		GLOBAL _io_hlt,_write_mem8			; 程序中包含的函数名

; 实际的函数

[SECTION .text]					; 目标文件中写了这些后再写程序

_io_hlt:						; void io_hlt(void)
		HLT
		RET
		
; 用汇编语言编写函数io_hlt

_write_mem8:					; 内存写入函数
		MOV 	ECX,[ESP+4]		; [ESP+4]中存放着地址，将其读入ECX
		MOV 	AL,[ESP+8]		; [ESP+8]中存放着数据，将其读入AL
		MOV		[ECX],AL		; 将[AL]中的数据存至目标地址ECX中
		RET