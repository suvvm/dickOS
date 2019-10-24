[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	EXTERN	_io_out8
	EXTERN	_io_in8
	EXTERN	_io_hlt
	EXTERN	_loadGdtr
	EXTERN	_loadIdtr
	EXTERN	_asm_interruptHandler21
	EXTERN	_asm_interruptHandler27
	EXTERN	_asm_interruptHandler2c
	EXTERN	_io_load_eflags
	EXTERN	_io_cli
	EXTERN	_io_store_eflags
	EXTERN	_font
	EXTERN	_io_sti
	EXTERN	_sprintf
	EXTERN	_io_stihlt
[FILE "bootpack.c"]
[SECTION .text]
	GLOBAL	_QueueInit
_QueueInit:
	PUSH	EBP
	MOV	EBP,ESP
	MOV	EAX,DWORD [8+EBP]
	MOV	ECX,DWORD [12+EBP]
	MOV	EDX,DWORD [16+EBP]
	MOV	DWORD [12+EAX],ECX
	MOV	DWORD [EAX],EDX
	MOV	DWORD [16+EAX],ECX
	MOV	DWORD [20+EAX],0
	MOV	DWORD [8+EAX],0
	MOV	DWORD [4+EAX],0
	POP	EBP
	RET
	GLOBAL	_QueuePush
_QueuePush:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EBX
	MOV	ECX,DWORD [8+EBP]
	MOV	EBX,DWORD [12+EBP]
	CMP	DWORD [16+ECX],0
	JNE	L3
	OR	EAX,-1
	MOV	DWORD [20+ECX],1
L2:
	POP	EBX
	POP	EBP
	RET
L3:
	MOV	EAX,DWORD [4+ECX]
	MOV	EDX,DWORD [ECX]
	MOV	BYTE [EAX+EDX*1],BL
	MOV	EAX,DWORD [4+ECX]
	DEC	DWORD [16+ECX]
	INC	EAX
	CDQ
	IDIV	DWORD [12+ECX]
	XOR	EAX,EAX
	MOV	DWORD [4+ECX],EDX
	JMP	L2
	GLOBAL	_QueuePop
_QueuePop:
	PUSH	EBP
	OR	EAX,-1
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	MOV	ESI,DWORD [8+EBP]
	PUSH	EBX
	MOV	EDI,DWORD [16+ESI]
	MOV	EBX,DWORD [12+ESI]
	CMP	EDI,EBX
	JE	L4
	MOV	EAX,DWORD [8+ESI]
	MOV	EDX,DWORD [ESI]
	MOVZX	ECX,BYTE [EAX+EDX*1]
	INC	EAX
	CDQ
	IDIV	EBX
	LEA	EAX,DWORD [1+EDI]
	MOV	DWORD [8+ESI],EDX
	MOV	DWORD [16+ESI],EAX
	MOV	EAX,ECX
L4:
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
	GLOBAL	_QueueSize
_QueueSize:
	PUSH	EBP
	MOV	EBP,ESP
	MOV	EDX,DWORD [8+EBP]
	POP	EBP
	MOV	EAX,DWORD [12+EDX]
	SUB	EAX,DWORD [16+EDX]
	RET
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
	GLOBAL	_interruptHandler21
_interruptHandler21:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	97
	PUSH	32
	CALL	_io_out8
	PUSH	96
	CALL	_io_in8
	MOVZX	EAX,AL
	PUSH	EAX
	PUSH	_keybuf
	CALL	_QueuePush
	LEAVE
	RET
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
LC0:
	DB	"INT 2C (IRQ-12) : PS/2 mouse",0x00
[SECTION .text]
	GLOBAL	_interruptHandler2c
_interruptHandler2c:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	15
	PUSH	255
	PUSH	0
	PUSH	0
	PUSH	0
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [4088]
	CALL	_boxFill8
	PUSH	LC0
	PUSH	7
	PUSH	0
	PUSH	0
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [4088]
	CALL	_putFont8_asc
	ADD	ESP,52
L11:
	CALL	_io_hlt
	JMP	L11
	GLOBAL	_setSegmdesc
_setSegmdesc:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EBX
	MOV	EDX,DWORD [12+EBP]
	MOV	ECX,DWORD [16+EBP]
	MOV	EBX,DWORD [8+EBP]
	MOV	EAX,DWORD [20+EBP]
	CMP	EDX,1048575
	JBE	L15
	SHR	EDX,12
	OR	EAX,32768
L15:
	MOV	WORD [EBX],DX
	MOV	BYTE [5+EBX],AL
	SHR	EDX,16
	SAR	EAX,8
	AND	EDX,15
	MOV	WORD [2+EBX],CX
	AND	EAX,-16
	SAR	ECX,16
	OR	EDX,EAX
	MOV	BYTE [4+EBX],CL
	MOV	BYTE [6+EBX],DL
	SAR	ECX,8
	MOV	BYTE [7+EBX],CL
	POP	EBX
	POP	EBP
	RET
	GLOBAL	_setGatedesc
_setGatedesc:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EBX
	MOV	EDX,DWORD [8+EBP]
	MOV	EAX,DWORD [16+EBP]
	MOV	EBX,DWORD [20+EBP]
	MOV	ECX,DWORD [12+EBP]
	MOV	WORD [2+EDX],AX
	MOV	BYTE [5+EDX],BL
	MOV	WORD [EDX],CX
	MOV	EAX,EBX
	SAR	EAX,8
	SAR	ECX,16
	MOV	BYTE [4+EDX],AL
	MOV	WORD [6+EDX],CX
	POP	EBX
	POP	EBP
	RET
	GLOBAL	_initGdtit
_initGdtit:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
	PUSH	EBX
	MOV	ESI,2555904
	MOV	EBX,8191
L22:
	PUSH	0
	PUSH	0
	PUSH	0
	PUSH	ESI
	ADD	ESI,8
	CALL	_setSegmdesc
	ADD	ESP,16
	DEC	EBX
	JNS	L22
	PUSH	16530
	MOV	ESI,2553856
	PUSH	0
	MOV	EBX,255
	PUSH	-1
	PUSH	2555912
	CALL	_setSegmdesc
	PUSH	16538
	PUSH	2621440
	PUSH	524287
	PUSH	2555920
	CALL	_setSegmdesc
	ADD	ESP,32
	PUSH	2555904
	PUSH	65535
	CALL	_loadGdtr
	POP	EAX
	POP	EDX
L27:
	PUSH	0
	PUSH	0
	PUSH	0
	PUSH	ESI
	ADD	ESI,8
	CALL	_setGatedesc
	ADD	ESP,16
	DEC	EBX
	JNS	L27
	PUSH	2553856
	PUSH	2047
	CALL	_loadIdtr
	PUSH	142
	PUSH	16
	PUSH	_asm_interruptHandler21
	PUSH	2554120
	CALL	_setGatedesc
	PUSH	142
	PUSH	16
	PUSH	_asm_interruptHandler27
	PUSH	2554168
	CALL	_setGatedesc
	ADD	ESP,40
	PUSH	142
	PUSH	16
	PUSH	_asm_interruptHandler2c
	PUSH	2554208
	CALL	_setGatedesc
	LEA	ESP,DWORD [-8+EBP]
	POP	EBX
	POP	ESI
	POP	EBP
	RET
	GLOBAL	_set_palette
_set_palette:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	PUSH	EAX
	MOV	EBX,DWORD [8+EBP]
	MOV	EDI,DWORD [12+EBP]
	MOV	ESI,DWORD [16+EBP]
	CALL	_io_load_eflags
	MOV	DWORD [-16+EBP],EAX
	CALL	_io_cli
	PUSH	EBX
	PUSH	968
	CALL	_io_out8
	CMP	EBX,EDI
	POP	ECX
	POP	EAX
	JGE	L39
	SUB	EDI,EBX
	MOV	EBX,EDI
L37:
	MOV	AL,BYTE [ESI]
	SHR	AL,2
	MOVZX	EAX,AL
	PUSH	EAX
	PUSH	969
	CALL	_io_out8
	MOV	AL,BYTE [1+ESI]
	SHR	AL,2
	MOVZX	EAX,AL
	PUSH	EAX
	PUSH	969
	CALL	_io_out8
	MOV	AL,BYTE [2+ESI]
	SHR	AL,2
	ADD	ESI,3
	MOVZX	EAX,AL
	PUSH	EAX
	PUSH	969
	CALL	_io_out8
	ADD	ESP,24
	DEC	EBX
	JNE	L37
L39:
	MOV	EAX,DWORD [-16+EBP]
	MOV	DWORD [8+EBP],EAX
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	JMP	_io_store_eflags
[SECTION .data]
_table_rgb.0:
	DB	0
	DB	0
	DB	0
	DB	-1
	DB	0
	DB	0
	DB	0
	DB	-1
	DB	0
	DB	-1
	DB	-1
	DB	0
	DB	0
	DB	0
	DB	-1
	DB	-1
	DB	0
	DB	-1
	DB	0
	DB	-1
	DB	-1
	DB	-1
	DB	-1
	DB	-1
	DB	-58
	DB	-58
	DB	-58
	DB	-124
	DB	0
	DB	0
	DB	0
	DB	-124
	DB	0
	DB	-124
	DB	-124
	DB	0
	DB	0
	DB	0
	DB	-124
	DB	-124
	DB	0
	DB	-124
	DB	0
	DB	-124
	DB	-124
	DB	-124
	DB	-124
	DB	-124
[SECTION .text]
	GLOBAL	_init_palette
_init_palette:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	_table_rgb.0
	PUSH	15
	PUSH	0
	CALL	_set_palette
	LEAVE
	RET
	GLOBAL	_boxFill8
_boxFill8:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	PUSH	ECX
	PUSH	ECX
	MOV	AL,BYTE [16+EBP]
	MOV	ECX,DWORD [24+EBP]
	MOV	EDI,DWORD [28+EBP]
	MOV	BYTE [-13+EBP],AL
	CMP	ECX,DWORD [32+EBP]
	JG	L53
	MOV	EBX,DWORD [12+EBP]
	IMUL	EBX,ECX
L51:
	MOV	EDX,DWORD [20+EBP]
	CMP	EDX,EDI
	JG	L55
	MOV	ESI,DWORD [8+EBP]
	ADD	ESI,EBX
	ADD	ESI,EDX
	MOV	DWORD [-20+EBP],ESI
L50:
	MOV	ESI,DWORD [-20+EBP]
	MOV	AL,BYTE [-13+EBP]
	INC	EDX
	MOV	BYTE [ESI],AL
	INC	ESI
	MOV	DWORD [-20+EBP],ESI
	CMP	EDX,EDI
	JLE	L50
L55:
	INC	ECX
	ADD	EBX,DWORD [12+EBP]
	CMP	ECX,DWORD [32+EBP]
	JLE	L51
L53:
	POP	EAX
	POP	EDX
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
	GLOBAL	_init_GUI
_init_GUI:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	SUB	ESP,12
	MOV	EAX,DWORD [16+EBP]
	MOV	EDI,DWORD [12+EBP]
	SUB	EAX,29
	DEC	EDI
	PUSH	EAX
	PUSH	EDI
	PUSH	0
	PUSH	0
	PUSH	14
	PUSH	DWORD [12+EBP]
	PUSH	DWORD [8+EBP]
	CALL	_boxFill8
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,28
	PUSH	EAX
	PUSH	EDI
	PUSH	EAX
	PUSH	0
	PUSH	8
	PUSH	DWORD [12+EBP]
	PUSH	DWORD [8+EBP]
	CALL	_boxFill8
	MOV	EAX,DWORD [16+EBP]
	ADD	ESP,56
	SUB	EAX,27
	PUSH	EAX
	PUSH	EDI
	PUSH	EAX
	PUSH	0
	PUSH	7
	PUSH	DWORD [12+EBP]
	PUSH	DWORD [8+EBP]
	CALL	_boxFill8
	MOV	EAX,DWORD [16+EBP]
	DEC	EAX
	PUSH	EAX
	MOV	EAX,DWORD [16+EBP]
	PUSH	EDI
	SUB	EAX,26
	PUSH	EAX
	PUSH	0
	PUSH	8
	PUSH	DWORD [12+EBP]
	PUSH	DWORD [8+EBP]
	CALL	_boxFill8
	MOV	ESI,DWORD [16+EBP]
	ADD	ESP,56
	SUB	ESI,24
	PUSH	ESI
	PUSH	59
	PUSH	ESI
	PUSH	3
	PUSH	7
	PUSH	DWORD [12+EBP]
	PUSH	DWORD [8+EBP]
	CALL	_boxFill8
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,4
	PUSH	EAX
	MOV	DWORD [-16+EBP],EAX
	PUSH	2
	PUSH	ESI
	PUSH	2
	PUSH	7
	PUSH	DWORD [12+EBP]
	PUSH	DWORD [8+EBP]
	CALL	_boxFill8
	ADD	ESP,56
	PUSH	DWORD [-16+EBP]
	PUSH	59
	PUSH	DWORD [-16+EBP]
	PUSH	3
	PUSH	15
	PUSH	DWORD [12+EBP]
	PUSH	DWORD [8+EBP]
	CALL	_boxFill8
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,5
	PUSH	EAX
	MOV	EAX,DWORD [16+EBP]
	PUSH	59
	SUB	EAX,23
	PUSH	EAX
	MOV	DWORD [-20+EBP],EAX
	PUSH	59
	PUSH	15
	PUSH	DWORD [12+EBP]
	PUSH	DWORD [8+EBP]
	CALL	_boxFill8
	MOV	EAX,DWORD [16+EBP]
	ADD	ESP,56
	SUB	EAX,3
	MOV	DWORD [-24+EBP],EAX
	PUSH	EAX
	PUSH	59
	PUSH	EAX
	PUSH	2
	PUSH	0
	PUSH	DWORD [12+EBP]
	PUSH	DWORD [8+EBP]
	CALL	_boxFill8
	PUSH	DWORD [-24+EBP]
	PUSH	60
	PUSH	ESI
	PUSH	60
	PUSH	0
	PUSH	DWORD [12+EBP]
	PUSH	DWORD [8+EBP]
	CALL	_boxFill8
	MOV	EDI,DWORD [12+EBP]
	ADD	ESP,56
	MOV	EBX,DWORD [12+EBP]
	SUB	EBX,4
	SUB	EDI,47
	PUSH	ESI
	PUSH	EBX
	PUSH	ESI
	PUSH	EDI
	PUSH	15
	PUSH	DWORD [12+EBP]
	PUSH	DWORD [8+EBP]
	CALL	_boxFill8
	PUSH	DWORD [-16+EBP]
	PUSH	EDI
	PUSH	DWORD [-20+EBP]
	PUSH	EDI
	PUSH	15
	PUSH	DWORD [12+EBP]
	PUSH	DWORD [8+EBP]
	CALL	_boxFill8
	ADD	ESP,56
	PUSH	DWORD [-24+EBP]
	PUSH	EBX
	PUSH	DWORD [-24+EBP]
	PUSH	EDI
	PUSH	7
	PUSH	DWORD [12+EBP]
	PUSH	DWORD [8+EBP]
	CALL	_boxFill8
	MOV	EAX,DWORD [12+EBP]
	PUSH	DWORD [-24+EBP]
	SUB	EAX,3
	PUSH	EAX
	PUSH	ESI
	PUSH	EAX
	PUSH	7
	PUSH	DWORD [12+EBP]
	PUSH	DWORD [8+EBP]
	CALL	_boxFill8
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
	GLOBAL	_putFont8
_putFont8:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	XOR	ESI,ESI
	PUSH	EBX
	MOV	EDI,DWORD [28+EBP]
	MOV	BL,BYTE [24+EBP]
L70:
	MOV	EAX,DWORD [20+EBP]
	MOV	EDX,DWORD [16+EBP]
	ADD	EAX,ESI
	IMUL	EAX,DWORD [12+EBP]
	ADD	EAX,DWORD [8+EBP]
	LEA	ECX,DWORD [EDX+EAX*1]
	MOV	DL,BYTE [ESI+EDI*1]
	TEST	DL,DL
	JNS	L62
	MOV	BYTE [ECX],BL
L62:
	MOV	AL,DL
	AND	EAX,64
	TEST	AL,AL
	JE	L63
	MOV	BYTE [1+ECX],BL
L63:
	MOV	AL,DL
	AND	EAX,32
	TEST	AL,AL
	JE	L64
	MOV	BYTE [2+ECX],BL
L64:
	MOV	AL,DL
	AND	EAX,16
	TEST	AL,AL
	JE	L65
	MOV	BYTE [3+ECX],BL
L65:
	MOV	AL,DL
	AND	EAX,8
	TEST	AL,AL
	JE	L66
	MOV	BYTE [4+ECX],BL
L66:
	MOV	AL,DL
	AND	EAX,4
	TEST	AL,AL
	JE	L67
	MOV	BYTE [5+ECX],BL
L67:
	MOV	AL,DL
	AND	EAX,2
	TEST	AL,AL
	JE	L68
	MOV	BYTE [6+ECX],BL
L68:
	AND	EDX,1
	TEST	DL,DL
	JE	L60
	MOV	BYTE [7+ECX],BL
L60:
	INC	ESI
	CMP	ESI,15
	JLE	L70
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
	GLOBAL	_putFont8_asc
_putFont8_asc:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	PUSH	EBX
	MOV	EBX,DWORD [28+EBP]
	MOV	AL,BYTE [24+EBP]
	MOV	BYTE [-13+EBP],AL
	MOV	ESI,DWORD [16+EBP]
	MOV	EDI,DWORD [20+EBP]
	CMP	BYTE [EBX],0
	JNE	L78
L80:
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
L78:
	MOVZX	EAX,BYTE [EBX]
	SAL	EAX,4
	INC	EBX
	ADD	EAX,_font
	PUSH	EAX
	MOVSX	EAX,BYTE [-13+EBP]
	PUSH	EAX
	PUSH	EDI
	PUSH	ESI
	ADD	ESI,8
	PUSH	DWORD [12+EBP]
	PUSH	DWORD [8+EBP]
	CALL	_putFont8
	ADD	ESP,24
	CMP	BYTE [EBX],0
	JNE	L78
	JMP	L80
[SECTION .data]
_cursor.1:
	DB	"*..............."
	DB	"**.............."
	DB	"*O*............."
	DB	"*OO*............"
	DB	"*OOO*..........."
	DB	"*OOOO*.........."
	DB	"*OOOOO*........."
	DB	"*OOOOOO*........"
	DB	"*OOOOOOO*......."
	DB	"*OOOO*****......"
	DB	"*OO*O*.........."
	DB	"*O*.*O*........."
	DB	"**..*O*........."
	DB	"*....*O*........"
	DB	".....*O*........"
	DB	"......*........."
[SECTION .text]
	GLOBAL	_initMouseCursor8
_initMouseCursor8:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	XOR	EBX,EBX
	PUSH	EDI
	XOR	EDI,EDI
	MOV	AL,BYTE [12+EBP]
	MOV	ESI,DWORD [8+EBP]
	MOV	BYTE [-13+EBP],AL
L94:
	XOR	EDX,EDX
L93:
	LEA	EAX,DWORD [EDX+EDI*1]
	CMP	BYTE [_cursor.1+EAX],42
	JE	L99
L90:
	CMP	BYTE [_cursor.1+EAX],79
	JE	L100
L91:
	CMP	BYTE [_cursor.1+EAX],46
	JE	L101
L88:
	INC	EDX
	CMP	EDX,15
	JLE	L93
	INC	EBX
	ADD	EDI,16
	CMP	EBX,15
	JLE	L94
	POP	ESI
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
L101:
	MOV	CL,BYTE [-13+EBP]
	MOV	BYTE [EAX+ESI*1],CL
	JMP	L88
L100:
	MOV	BYTE [EAX+ESI*1],7
	JMP	L91
L99:
	MOV	BYTE [EAX+ESI*1],0
	JMP	L90
	GLOBAL	_putblock8_8
_putblock8_8:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	XOR	ESI,ESI
	PUSH	EBX
	SUB	ESP,12
	CMP	ESI,DWORD [20+EBP]
	JGE	L114
	XOR	EDI,EDI
L112:
	XOR	EBX,EBX
	CMP	EBX,DWORD [16+EBP]
	JGE	L116
	MOV	EAX,DWORD [32+EBP]
	ADD	EAX,EDI
	MOV	DWORD [-20+EBP],EAX
L111:
	MOV	EAX,DWORD [28+EBP]
	MOV	EDX,DWORD [24+EBP]
	ADD	EAX,ESI
	ADD	EDX,EBX
	IMUL	EAX,DWORD [12+EBP]
	ADD	EAX,EDX
	MOV	ECX,DWORD [8+EBP]
	MOV	EDX,DWORD [-20+EBP]
	INC	EBX
	MOV	DL,BYTE [EDX]
	MOV	BYTE [EAX+ECX*1],DL
	INC	DWORD [-20+EBP]
	CMP	EBX,DWORD [16+EBP]
	JL	L111
L116:
	INC	ESI
	ADD	EDI,DWORD [36+EBP]
	CMP	ESI,DWORD [20+EBP]
	JL	L112
L114:
	ADD	ESP,12
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
	GLOBAL	_waitKeyboardControllerReady
_waitKeyboardControllerReady:
	PUSH	EBP
	MOV	EBP,ESP
L118:
	PUSH	100
	CALL	_io_in8
	POP	EDX
	AND	EAX,2
	JNE	L118
	LEAVE
	RET
	GLOBAL	_initKeyboard
_initKeyboard:
	PUSH	EBP
	MOV	EBP,ESP
	CALL	_waitKeyboardControllerReady
	PUSH	96
	PUSH	100
	CALL	_io_out8
	CALL	_waitKeyboardControllerReady
	PUSH	71
	PUSH	96
	CALL	_io_out8
	LEAVE
	RET
	GLOBAL	_enableMouse
_enableMouse:
	PUSH	EBP
	MOV	EBP,ESP
	CALL	_waitKeyboardControllerReady
	PUSH	212
	PUSH	100
	CALL	_io_out8
	CALL	_waitKeyboardControllerReady
	PUSH	244
	PUSH	96
	CALL	_io_out8
	LEAVE
	RET
[SECTION .data]
LC1:
	DB	"SHOWSTRING",0x00
LC2:
	DB	"(%d, %d)",0x00
LC3:
	DB	"%02X",0x00
[SECTION .text]
	GLOBAL	_Main
_Main:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	SUB	ESP,340
	CALL	_initGdtit
	CALL	_init_pic
	CALL	_io_sti
	LEA	EAX,DWORD [-348+EBP]
	PUSH	EAX
	PUSH	32
	PUSH	_keybuf
	CALL	_QueueInit
	PUSH	249
	PUSH	33
	CALL	_io_out8
	PUSH	239
	PUSH	161
	CALL	_io_out8
	CALL	_initKeyboard
	CALL	_init_palette
	MOV	ECX,2
	MOVSX	EAX,WORD [4084]
	MOVSX	EBX,WORD [4086]
	LEA	EDX,DWORD [-16+EAX]
	PUSH	EBX
	MOV	EAX,EDX
	LEA	ESI,DWORD [-44+EBX]
	CDQ
	IDIV	ECX
	MOV	EDI,EAX
	LEA	EBX,DWORD [-268+EBP]
	MOV	EAX,ESI
	CDQ
	IDIV	ECX
	MOV	ESI,EAX
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [4088]
	CALL	_init_GUI
	ADD	ESP,40
	PUSH	LC1
	PUSH	7
	PUSH	30
	PUSH	30
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [4088]
	CALL	_putFont8_asc
	PUSH	14
	PUSH	EBX
	CALL	_initMouseCursor8
	ADD	ESP,32
	PUSH	16
	PUSH	EBX
	LEA	EBX,DWORD [-316+EBP]
	PUSH	ESI
	PUSH	EDI
	PUSH	16
	PUSH	16
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [4088]
	CALL	_putblock8_8
	ADD	ESP,32
	PUSH	ESI
	PUSH	EDI
	PUSH	LC2
	PUSH	EBX
	CALL	_sprintf
	PUSH	EBX
	PUSH	7
	PUSH	0
	PUSH	0
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [4088]
	CALL	_putFont8_asc
	ADD	ESP,40
	PUSH	249
	PUSH	33
	CALL	_io_out8
	PUSH	239
	PUSH	161
	CALL	_io_out8
	CALL	_enableMouse
	ADD	ESP,16
L125:
	CALL	_io_cli
	PUSH	_keybuf
	CALL	_QueueSize
	POP	ECX
	TEST	EAX,EAX
	JE	L130
	PUSH	_keybuf
	CALL	_QueuePop
	MOV	EBX,EAX
	CALL	_io_sti
	PUSH	EBX
	LEA	EBX,DWORD [-316+EBP]
	PUSH	LC3
	PUSH	EBX
	CALL	_sprintf
	PUSH	31
	PUSH	15
	PUSH	16
	PUSH	0
	PUSH	14
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [4088]
	CALL	_boxFill8
	ADD	ESP,44
	PUSH	EBX
	PUSH	7
	PUSH	16
	PUSH	0
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [4088]
	CALL	_putFont8_asc
	ADD	ESP,24
	JMP	L125
L130:
	CALL	_io_stihlt
	JMP	L125
	GLOBAL	_keybuf
[SECTION .data]
	ALIGNB	16
_keybuf:
	RESB	24
