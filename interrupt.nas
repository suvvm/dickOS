[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	EXTERN	_io_out8
	EXTERN	_putFont8_asc
	EXTERN	_boxFill8
	EXTERN	_io_hlt
[FILE "interrupt.c"]
[SECTION .text]
	GLOBAL	_init_pic
_init_pic:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	255
	PUSH	33
	CALL	_io_out8
	PUSH	255
	PUSH	161
	CALL	_io_out8
	PUSH	17
	PUSH	32
	CALL	_io_out8
	PUSH	32
	PUSH	33
	CALL	_io_out8
	ADD	ESP,32
	PUSH	4
	PUSH	33
	CALL	_io_out8
	PUSH	1
	PUSH	33
	CALL	_io_out8
	PUSH	17
	PUSH	160
	CALL	_io_out8
	PUSH	40
	PUSH	161
	CALL	_io_out8
	ADD	ESP,32
	PUSH	2
	PUSH	161
	CALL	_io_out8
	PUSH	1
	PUSH	161
	CALL	_io_out8
	PUSH	251
	PUSH	33
	CALL	_io_out8
	PUSH	255
	PUSH	161
	CALL	_io_out8
	LEAVE
	RET
[SECTION .data]
LC0:
	DB	"INT 21 (IRQ-1) : PS/2 keyboard",0x00
[SECTION .text]
	GLOBAL	_interruptHandler21
_interruptHandler21:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	LC0
	PUSH	7
	PUSH	0
	PUSH	0
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [4088]
	CALL	_putFont8_asc
	PUSH	15
	PUSH	255
	PUSH	0
	PUSH	0
	PUSH	0
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [4088]
	CALL	_boxFill8
	ADD	ESP,52
L3:
	CALL	_io_hlt
	JMP	L3
	GLOBAL	_interruptHandler27
_interruptHandler27:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	103
	PUSH	32
	CALL	_io_out8
	LEAVE
	RET
[SECTION .data]
LC1:
	DB	"INT 2C (IRQ-12) : PS/2 mouse",0x00
[SECTION .text]
	GLOBAL	_interruptHandler2c
_interruptHandler2c:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	LC1
	PUSH	7
	PUSH	0
	PUSH	0
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [4088]
	CALL	_putFont8_asc
	PUSH	15
	PUSH	255
	PUSH	0
	PUSH	0
	PUSH	0
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [4088]
	CALL	_boxFill8
	ADD	ESP,52
L8:
	CALL	_io_hlt
	JMP	L8
