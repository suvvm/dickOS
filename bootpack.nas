[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	EXTERN	_io_out8
	EXTERN	_io_in8
	EXTERN	_loadGdtr
	EXTERN	_loadIdtr
	EXTERN	_asm_interruptHandler21
	EXTERN	_asm_interruptHandler27
	EXTERN	_asm_interruptHandler2c
	EXTERN	_io_load_eflags
	EXTERN	_io_cli
	EXTERN	_io_store_eflags
	EXTERN	_font
	EXTERN	_memtest_sub
	EXTERN	_loadCr0
	EXTERN	_storeCr0
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
	GLOBAL	_interruptHandler2c
_interruptHandler2c:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	100
	PUSH	160
	CALL	_io_out8
	PUSH	98
	PUSH	32
	CALL	_io_out8
	PUSH	96
	CALL	_io_in8
	MOVZX	EAX,AL
	PUSH	EAX
	PUSH	_mousebuf
	CALL	_QueuePush
	LEAVE
	RET
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
	JBE	L12
	SHR	EDX,12
	OR	EAX,32768
L12:
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
L19:
	PUSH	0
	PUSH	0
	PUSH	0
	PUSH	ESI
	ADD	ESI,8
	CALL	_setSegmdesc
	ADD	ESP,16
	DEC	EBX
	JNS	L19
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
L24:
	PUSH	0
	PUSH	0
	PUSH	0
	PUSH	ESI
	ADD	ESI,8
	CALL	_setGatedesc
	ADD	ESP,16
	DEC	EBX
	JNS	L24
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
	JGE	L36
	SUB	EDI,EBX
	MOV	EBX,EDI
L34:
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
	JNE	L34
L36:
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
	JG	L50
	MOV	EBX,DWORD [12+EBP]
	IMUL	EBX,ECX
L48:
	MOV	EDX,DWORD [20+EBP]
	CMP	EDX,EDI
	JG	L52
	MOV	ESI,DWORD [8+EBP]
	ADD	ESI,EBX
	ADD	ESI,EDX
	MOV	DWORD [-20+EBP],ESI
L47:
	MOV	ESI,DWORD [-20+EBP]
	MOV	AL,BYTE [-13+EBP]
	INC	EDX
	MOV	BYTE [ESI],AL
	INC	ESI
	MOV	DWORD [-20+EBP],ESI
	CMP	EDX,EDI
	JLE	L47
L52:
	INC	ECX
	ADD	EBX,DWORD [12+EBP]
	CMP	ECX,DWORD [32+EBP]
	JLE	L48
L50:
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
L67:
	MOV	EAX,DWORD [20+EBP]
	MOV	EDX,DWORD [16+EBP]
	ADD	EAX,ESI
	IMUL	EAX,DWORD [12+EBP]
	ADD	EAX,DWORD [8+EBP]
	LEA	ECX,DWORD [EDX+EAX*1]
	MOV	DL,BYTE [ESI+EDI*1]
	TEST	DL,DL
	JNS	L59
	MOV	BYTE [ECX],BL
L59:
	MOV	AL,DL
	AND	EAX,64
	TEST	AL,AL
	JE	L60
	MOV	BYTE [1+ECX],BL
L60:
	MOV	AL,DL
	AND	EAX,32
	TEST	AL,AL
	JE	L61
	MOV	BYTE [2+ECX],BL
L61:
	MOV	AL,DL
	AND	EAX,16
	TEST	AL,AL
	JE	L62
	MOV	BYTE [3+ECX],BL
L62:
	MOV	AL,DL
	AND	EAX,8
	TEST	AL,AL
	JE	L63
	MOV	BYTE [4+ECX],BL
L63:
	MOV	AL,DL
	AND	EAX,4
	TEST	AL,AL
	JE	L64
	MOV	BYTE [5+ECX],BL
L64:
	MOV	AL,DL
	AND	EAX,2
	TEST	AL,AL
	JE	L65
	MOV	BYTE [6+ECX],BL
L65:
	AND	EDX,1
	TEST	DL,DL
	JE	L57
	MOV	BYTE [7+ECX],BL
L57:
	INC	ESI
	CMP	ESI,15
	JLE	L67
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
	JNE	L75
L77:
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
L75:
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
	JNE	L75
	JMP	L77
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
L91:
	XOR	EDX,EDX
L90:
	LEA	EAX,DWORD [EDX+EDI*1]
	CMP	BYTE [_cursor.1+EAX],42
	JE	L96
L87:
	CMP	BYTE [_cursor.1+EAX],79
	JE	L97
L88:
	CMP	BYTE [_cursor.1+EAX],46
	JE	L98
L85:
	INC	EDX
	CMP	EDX,15
	JLE	L90
	INC	EBX
	ADD	EDI,16
	CMP	EBX,15
	JLE	L91
	POP	ESI
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
L98:
	MOV	CL,BYTE [-13+EBP]
	MOV	BYTE [EAX+ESI*1],CL
	JMP	L85
L97:
	MOV	BYTE [EAX+ESI*1],7
	JMP	L88
L96:
	MOV	BYTE [EAX+ESI*1],0
	JMP	L87
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
	JGE	L111
	XOR	EDI,EDI
L109:
	XOR	EBX,EBX
	CMP	EBX,DWORD [16+EBP]
	JGE	L113
	MOV	EAX,DWORD [32+EBP]
	ADD	EAX,EDI
	MOV	DWORD [-20+EBP],EAX
L108:
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
	JL	L108
L113:
	INC	ESI
	ADD	EDI,DWORD [36+EBP]
	CMP	ESI,DWORD [20+EBP]
	JL	L109
L111:
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
L115:
	PUSH	100
	CALL	_io_in8
	POP	EDX
	AND	EAX,2
	JNE	L115
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
	MOV	EAX,DWORD [8+EBP]
	MOV	BYTE [3+EAX],0
	LEAVE
	RET
	GLOBAL	_mouseDecode
_mouseDecode:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
	PUSH	EBX
	MOV	EDX,DWORD [8+EBP]
	MOV	ECX,DWORD [12+EBP]
	MOV	ESI,ECX
	MOV	AL,BYTE [3+EDX]
	TEST	AL,AL
	JNE	L122
	CMP	CL,-6
	JE	L131
L130:
	XOR	EAX,EAX
L121:
	POP	EBX
	POP	ESI
	POP	EBP
	RET
L131:
	MOV	BYTE [3+EDX],1
	JMP	L130
L122:
	CMP	AL,1
	JE	L132
	CMP	AL,2
	JE	L133
	CMP	AL,3
	JE	L134
	OR	EAX,-1
	JMP	L121
L134:
	MOV	BL,BYTE [EDX]
	AND	ESI,255
	MOV	EAX,EBX
	MOV	BYTE [2+EDX],CL
	AND	EAX,7
	MOV	DWORD [8+EDX],ESI
	MOV	DWORD [12+EDX],EAX
	MOV	AL,BL
	MOVZX	ECX,BYTE [1+EDX]
	AND	EAX,16
	MOV	DWORD [4+EDX],ECX
	MOV	BYTE [3+EDX],1
	TEST	AL,AL
	JE	L128
	OR	ECX,-256
	MOV	DWORD [4+EDX],ECX
L128:
	AND	EBX,32
	TEST	BL,BL
	JE	L129
	OR	DWORD [8+EDX],-256
L129:
	NEG	DWORD [8+EDX]
	MOV	EAX,1
	JMP	L121
L133:
	MOV	BYTE [1+EDX],CL
	MOV	BYTE [3+EDX],3
	JMP	L130
L132:
	AND	ESI,-56
	MOV	EAX,ESI
	CMP	AL,8
	JNE	L130
	MOV	BYTE [EDX],CL
	MOV	BYTE [3+EDX],2
	JMP	L130
	GLOBAL	_memtest
_memtest:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
	PUSH	EBX
	XOR	ESI,ESI
	CALL	_io_load_eflags
	OR	EAX,262144
	PUSH	EAX
	CALL	_io_store_eflags
	CALL	_io_load_eflags
	POP	EDX
	TEST	EAX,262144
	JE	L136
	MOV	ESI,1
L136:
	AND	EAX,-262145
	PUSH	EAX
	CALL	_io_store_eflags
	MOV	EAX,ESI
	POP	EBX
	TEST	AL,AL
	JNE	L139
L137:
	PUSH	DWORD [12+EBP]
	PUSH	DWORD [8+EBP]
	CALL	_memtest_sub
	MOV	EBX,EAX
	POP	EAX
	MOV	EAX,ESI
	POP	EDX
	TEST	AL,AL
	JNE	L140
L138:
	LEA	ESP,DWORD [-8+EBP]
	MOV	EAX,EBX
	POP	EBX
	POP	ESI
	POP	EBP
	RET
L140:
	CALL	_loadCr0
	AND	EAX,-1610612737
	PUSH	EAX
	CALL	_storeCr0
	POP	ECX
	JMP	L138
L139:
	CALL	_loadCr0
	OR	EAX,1610612736
	PUSH	EAX
	CALL	_storeCr0
	POP	ECX
	JMP	L137
	GLOBAL	_memsegTotal
_memsegTotal:
	PUSH	EBP
	XOR	EAX,EAX
	MOV	EBP,ESP
	XOR	EDX,EDX
	PUSH	EBX
	MOV	EBX,DWORD [8+EBP]
	MOV	ECX,DWORD [EBX]
	CMP	EAX,ECX
	JAE	L148
L146:
	ADD	EAX,DWORD [20+EBX+EDX*8]
	INC	EDX
	CMP	EDX,ECX
	JB	L146
L148:
	POP	EBX
	POP	EBP
	RET
	GLOBAL	_memsegInit
_memsegInit:
	PUSH	EBP
	MOV	EBP,ESP
	MOV	EAX,DWORD [8+EBP]
	MOV	DWORD [EAX],0
	MOV	DWORD [4+EAX],0
	MOV	DWORD [8+EAX],0
	MOV	DWORD [12+EAX],0
	POP	EBP
	RET
	GLOBAL	_memsegAlloc
_memsegAlloc:
	PUSH	EBP
	XOR	ECX,ECX
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	MOV	ESI,DWORD [12+EBP]
	MOV	EBX,DWORD [8+EBP]
	MOV	EAX,DWORD [EBX]
	CMP	ECX,EAX
	JAE	L164
L162:
	MOV	EDX,DWORD [20+EBX+ECX*8]
	CMP	EDX,ESI
	JAE	L166
	INC	ECX
	CMP	ECX,EAX
	JB	L162
L164:
	XOR	EAX,EAX
L150:
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
L166:
	MOV	EDI,DWORD [16+EBX+ECX*8]
	LEA	EAX,DWORD [ESI+EDI*1]
	MOV	DWORD [16+EBX+ECX*8],EAX
	MOV	EAX,EDX
	SUB	EAX,ESI
	MOV	DWORD [20+EBX+ECX*8],EAX
	TEST	EAX,EAX
	JNE	L156
	MOV	EAX,DWORD [EBX]
	DEC	EAX
	MOV	DWORD [EBX],EAX
	CMP	ECX,EAX
	JAE	L156
	MOV	ESI,EAX
L161:
	MOV	EAX,DWORD [24+EBX+ECX*8]
	MOV	EDX,DWORD [28+EBX+ECX*8]
	MOV	DWORD [16+EBX+ECX*8],EAX
	MOV	DWORD [20+EBX+ECX*8],EDX
	INC	ECX
	CMP	ECX,ESI
	JB	L161
L156:
	MOV	EAX,EDI
	JMP	L150
	GLOBAL	_memsegFree
_memsegFree:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	MOV	ESI,DWORD [8+EBP]
	PUSH	EBX
	XOR	EBX,EBX
	MOV	EDI,DWORD [ESI]
	CMP	EBX,EDI
	JGE	L169
L173:
	MOV	EAX,DWORD [12+EBP]
	CMP	DWORD [16+ESI+EBX*8],EAX
	JA	L169
	INC	EBX
	CMP	EBX,EDI
	JL	L173
L169:
	TEST	EBX,EBX
	JLE	L174
	MOV	EDX,DWORD [12+ESI+EBX*8]
	MOV	EAX,DWORD [8+ESI+EBX*8]
	ADD	EAX,EDX
	CMP	EAX,DWORD [12+EBP]
	JE	L194
L174:
	CMP	EBX,EDI
	JGE	L181
	MOV	EAX,DWORD [12+EBP]
	ADD	EAX,DWORD [16+EBP]
	CMP	EAX,DWORD [16+ESI+EBX*8]
	JE	L195
L181:
	CMP	EDI,4089
	JG	L182
	MOV	ECX,EDI
	CMP	EDI,EBX
	JLE	L192
L187:
	MOV	EAX,DWORD [8+ESI+ECX*8]
	MOV	EDX,DWORD [12+ESI+ECX*8]
	MOV	DWORD [16+ESI+ECX*8],EAX
	MOV	DWORD [20+ESI+ECX*8],EDX
	DEC	ECX
	CMP	ECX,EBX
	JG	L187
L192:
	LEA	EAX,DWORD [1+EDI]
	MOV	DWORD [ESI],EAX
	CMP	DWORD [4+ESI],EAX
	JGE	L188
	MOV	DWORD [4+ESI],EAX
L188:
	MOV	EAX,DWORD [12+EBP]
	MOV	DWORD [16+ESI+EBX*8],EAX
	MOV	EAX,DWORD [16+EBP]
	MOV	DWORD [20+ESI+EBX*8],EAX
L193:
	XOR	EAX,EAX
L167:
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
L182:
	MOV	EAX,DWORD [16+EBP]
	INC	DWORD [12+ESI]
	ADD	DWORD [8+ESI],EAX
	OR	EAX,-1
	JMP	L167
L195:
	MOV	EAX,DWORD [12+EBP]
	MOV	DWORD [16+ESI+EBX*8],EAX
	MOV	EAX,DWORD [16+EBP]
	ADD	DWORD [20+ESI+EBX*8],EAX
	JMP	L193
L194:
	ADD	EDX,DWORD [16+EBP]
	MOV	DWORD [12+ESI+EBX*8],EDX
	CMP	EBX,DWORD [ESI]
	JGE	L193
	MOV	EAX,DWORD [12+EBP]
	ADD	EAX,DWORD [16+EBP]
	CMP	EAX,DWORD [16+ESI+EBX*8]
	JNE	L193
	ADD	EDX,DWORD [20+ESI+EBX*8]
	MOV	DWORD [12+ESI+EBX*8],EDX
	MOV	EAX,DWORD [ESI]
	DEC	EAX
	MOV	DWORD [ESI],EAX
	CMP	EBX,EAX
	JGE	L193
	MOV	ECX,EAX
L180:
	MOV	EAX,DWORD [24+ESI+EBX*8]
	MOV	EDX,DWORD [28+ESI+EBX*8]
	MOV	DWORD [16+ESI+EBX*8],EAX
	MOV	DWORD [20+ESI+EBX*8],EDX
	INC	EBX
	CMP	EBX,ECX
	JL	L180
	JMP	L193
	GLOBAL	_memsegAlloc4K
_memsegAlloc4K:
	PUSH	EBP
	MOV	EBP,ESP
	MOV	EAX,DWORD [12+EBP]
	ADD	EAX,4095
	AND	EAX,-4096
	MOV	DWORD [12+EBP],EAX
	POP	EBP
	JMP	_memsegAlloc
	GLOBAL	_memsegFree4K
_memsegFree4K:
	PUSH	EBP
	MOV	EBP,ESP
	MOV	EAX,DWORD [16+EBP]
	ADD	EAX,4095
	AND	EAX,-4096
	MOV	DWORD [16+EBP],EAX
	POP	EBP
	JMP	_memsegFree
	GLOBAL	_shtctlInit
_shtctlInit:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	10256
	PUSH	DWORD [8+EBP]
	CALL	_memsegAlloc4K
	POP	ECX
	MOV	EDX,EAX
	POP	EAX
	XOR	EAX,EAX
	TEST	EDX,EDX
	JE	L198
	MOV	EAX,DWORD [12+EBP]
	LEA	ECX,DWORD [9180+EDX]
	MOV	DWORD [EDX],EAX
	MOV	EAX,DWORD [16+EBP]
	MOV	DWORD [4+EDX],EAX
	MOV	EAX,DWORD [20+EBP]
	MOV	DWORD [8+EDX],EAX
	MOV	EAX,EDX
	MOV	DWORD [12+EDX],-1
L204:
	MOV	DWORD [1068+EAX],0
	MOV	DWORD [1072+EAX],EDX
	ADD	EAX,36
	CMP	EAX,ECX
	JLE	L204
	MOV	EAX,EDX
L198:
	LEAVE
	RET
	GLOBAL	_sheetAlloc
_sheetAlloc:
	PUSH	EBP
	XOR	EDX,EDX
	MOV	EBP,ESP
	MOV	EAX,DWORD [8+EBP]
	ADD	EAX,1040
L213:
	CMP	DWORD [28+EAX],0
	JE	L216
	INC	EDX
	ADD	EAX,36
	CMP	EDX,255
	JLE	L213
	XOR	EAX,EAX
L207:
	POP	EBP
	RET
L216:
	MOV	DWORD [28+EAX],1
	MOV	DWORD [24+EAX],-1
	JMP	L207
	GLOBAL	_sheetSetbuf
_sheetSetbuf:
	PUSH	EBP
	MOV	EBP,ESP
	MOV	EDX,DWORD [8+EBP]
	MOV	EAX,DWORD [12+EBP]
	MOV	DWORD [EDX],EAX
	MOV	EAX,DWORD [16+EBP]
	MOV	DWORD [4+EDX],EAX
	MOV	EAX,DWORD [20+EBP]
	MOV	DWORD [8+EDX],EAX
	MOV	EAX,DWORD [24+EBP]
	MOV	DWORD [20+EDX],EAX
	POP	EBP
	RET
	GLOBAL	_sheetUpdown
_sheetUpdown:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	MOV	EDI,DWORD [8+EBP]
	PUSH	EBX
	MOV	EBX,DWORD [12+EBP]
	MOV	ESI,DWORD [32+EDI]
	MOV	EAX,DWORD [24+EDI]
	MOV	EDX,DWORD [12+ESI]
	INC	EDX
	CMP	EBX,EDX
	JLE	L219
	MOV	EBX,EDX
L219:
	CMP	EBX,-1
	JGE	L220
	OR	EBX,-1
L220:
	MOV	DWORD [24+EDI],EBX
	CMP	EAX,EBX
	JLE	L221
	TEST	EBX,EBX
	JS	L222
	MOV	ECX,EAX
L227:
	MOV	EAX,DWORD [12+ESI+ECX*4]
	MOV	DWORD [16+ESI+ECX*4],EAX
	MOV	DWORD [24+EAX],ECX
	DEC	ECX
	CMP	ECX,EBX
	JG	L227
L254:
	MOV	DWORD [16+ESI+EBX*4],EDI
L243:
	MOV	ECX,DWORD [16+EDI]
	MOV	EAX,ECX
	ADD	EAX,DWORD [8+EDI]
	PUSH	EAX
	MOV	EDX,DWORD [12+EDI]
	MOV	EAX,EDX
	ADD	EAX,DWORD [4+EDI]
	PUSH	EAX
	PUSH	ECX
	PUSH	EDX
	PUSH	EDI
	CALL	_sheetRefresh
	ADD	ESP,20
L218:
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
L222:
	MOV	EDX,DWORD [12+ESI]
	CMP	EDX,EAX
	JLE	L243
	MOV	ECX,EAX
	CMP	EAX,EDX
	JGE	L252
L234:
	MOV	EAX,DWORD [20+ESI+ECX*4]
	MOV	DWORD [16+ESI+ECX*4],EAX
	MOV	DWORD [24+EAX],ECX
	INC	ECX
	CMP	ECX,DWORD [12+ESI]
	JL	L234
L252:
	DEC	DWORD [12+ESI]
	JMP	L243
L221:
	CMP	EAX,EBX
	JGE	L218
	TEST	EAX,EAX
	JS	L237
	MOV	ECX,EAX
	CMP	EAX,EBX
	JGE	L254
L242:
	MOV	EAX,DWORD [20+ESI+ECX*4]
	MOV	DWORD [16+ESI+ECX*4],EAX
	MOV	DWORD [24+EAX],ECX
	INC	ECX
	CMP	ECX,EBX
	JL	L242
	JMP	L254
L237:
	MOV	ECX,DWORD [12+ESI]
	CMP	ECX,EBX
	JL	L256
L248:
	MOV	EDX,DWORD [16+ESI+ECX*4]
	LEA	EAX,DWORD [1+ECX]
	MOV	DWORD [20+ESI+ECX*4],EDX
	DEC	ECX
	MOV	DWORD [24+EDX],EAX
	CMP	ECX,EBX
	JGE	L248
L256:
	MOV	DWORD [16+ESI+EBX*4],EDI
	INC	DWORD [12+ESI]
	JMP	L243
	GLOBAL	_sheetRefresh
_sheetRefresh:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	PUSH	EAX
	MOV	EBX,DWORD [8+EBP]
	MOV	EAX,DWORD [12+EBP]
	MOV	DWORD [-16+EBP],EAX
	MOV	EDI,DWORD [16+EBP]
	MOV	ESI,DWORD [20+EBP]
	MOV	EAX,DWORD [24+EBP]
	CMP	DWORD [24+EBX],0
	JS	L257
	MOV	ECX,DWORD [16+EBX]
	MOV	EDX,DWORD [12+EBX]
	LEA	EAX,DWORD [EAX+ECX*1]
	ADD	ECX,EDI
	MOV	DWORD [24+EBP],EAX
	MOV	DWORD [16+EBP],ECX
	LEA	EAX,DWORD [ESI+EDX*1]
	ADD	EDX,DWORD [-16+EBP]
	MOV	DWORD [20+EBP],EAX
	MOV	EAX,DWORD [32+EBX]
	MOV	DWORD [12+EBP],EDX
	MOV	DWORD [8+EBP],EAX
	POP	EAX
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	JMP	_sheetRefreshSub
L257:
	POP	EAX
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
	GLOBAL	_sheetRefreshSub
_sheetRefreshSub:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	SUB	ESP,40
	MOV	EAX,DWORD [8+EBP]
	CMP	DWORD [12+EBP],0
	MOV	EAX,DWORD [EAX]
	MOV	DWORD [-40+EBP],EAX
	JS	L290
L260:
	CMP	DWORD [16+EBP],0
	JS	L291
L261:
	MOV	EDX,DWORD [8+EBP]
	MOV	EAX,DWORD [4+EDX]
	CMP	DWORD [20+EBP],EAX
	JLE	L262
	MOV	DWORD [20+EBP],EAX
L262:
	MOV	ESI,DWORD [8+EBP]
	MOV	EAX,DWORD [8+ESI]
	CMP	DWORD [24+EBP],EAX
	JLE	L263
	MOV	DWORD [24+EBP],EAX
L263:
	MOV	EAX,DWORD [8+EBP]
	XOR	EDI,EDI
	MOV	DWORD [-16+EBP],0
	CMP	EDI,DWORD [12+EAX]
	JG	L285
L283:
	MOV	EDX,DWORD [-16+EBP]
	MOV	ESI,DWORD [8+EBP]
	MOV	EBX,DWORD [16+EBP]
	MOV	ECX,DWORD [16+ESI+EDX*4]
	MOV	ESI,DWORD [20+EBP]
	MOV	EDI,DWORD [ECX]
	MOV	EDX,DWORD [16+ECX]
	MOV	DWORD [-32+EBP],EDI
	MOV	EAX,DWORD [12+ECX]
	MOV	EDI,DWORD [24+EBP]
	SUB	EBX,EDX
	SUB	EDI,EDX
	SUB	ESI,EAX
	MOV	EDX,DWORD [12+EBP]
	MOV	DWORD [-52+EBP],ESI
	SUB	EDX,EAX
	MOV	DWORD [-44+EBP],EDI
	MOV	DWORD [-28+EBP],EDX
	JS	L292
L268:
	TEST	EBX,EBX
	JS	L293
L269:
	MOV	EAX,DWORD [4+ECX]
	CMP	DWORD [-52+EBP],EAX
	JLE	L270
	MOV	DWORD [-52+EBP],EAX
L270:
	MOV	EAX,DWORD [8+ECX]
	CMP	DWORD [-44+EBP],EAX
	JLE	L271
	MOV	DWORD [-44+EBP],EAX
L271:
	CMP	EBX,DWORD [-44+EBP]
	JGE	L287
L282:
	MOV	ESI,DWORD [16+ECX]
	MOV	EDX,DWORD [-28+EBP]
	ADD	ESI,EBX
	CMP	EDX,DWORD [-52+EBP]
	MOV	DWORD [-24+EBP],ESI
	JGE	L289
L281:
	MOV	EAX,EBX
	MOV	EDI,DWORD [12+ECX]
	IMUL	EAX,DWORD [4+ECX]
	ADD	EDI,EDX
	MOV	ESI,DWORD [-32+EBP]
	ADD	EAX,EDX
	MOV	DWORD [-20+EBP],EDI
	MOV	AL,BYTE [EAX+ESI*1]
	MOV	BYTE [-33+EBP],AL
	MOVZX	EAX,AL
	CMP	EAX,DWORD [20+ECX]
	JE	L278
	MOV	EDI,DWORD [8+EBP]
	MOV	EAX,DWORD [-24+EBP]
	MOV	ESI,DWORD [-40+EBP]
	IMUL	EAX,DWORD [4+EDI]
	ADD	EAX,DWORD [-20+EBP]
	MOV	DWORD [-48+EBP],EAX
	MOV	AL,BYTE [-33+EBP]
	MOV	EDI,DWORD [-48+EBP]
	MOV	BYTE [EDI+ESI*1],AL
L278:
	INC	EDX
	CMP	EDX,DWORD [-52+EBP]
	JL	L281
L289:
	INC	EBX
	CMP	EBX,DWORD [-44+EBP]
	JL	L282
L287:
	MOV	EDX,DWORD [8+EBP]
	INC	DWORD [-16+EBP]
	MOV	EAX,DWORD [-16+EBP]
	CMP	EAX,DWORD [12+EDX]
	JLE	L283
L285:
	ADD	ESP,40
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
L293:
	XOR	EBX,EBX
	JMP	L269
L292:
	MOV	DWORD [-28+EBP],0
	JMP	L268
L291:
	MOV	DWORD [16+EBP],0
	JMP	L261
L290:
	MOV	DWORD [12+EBP],0
	JMP	L260
	GLOBAL	_sheetSlide
_sheetSlide:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	MOV	EDI,DWORD [12+EBP]
	MOV	EBX,DWORD [8+EBP]
	MOV	ESI,DWORD [16+EBP]
	MOV	ECX,DWORD [12+EBX]
	MOV	EDX,DWORD [16+EBX]
	MOV	DWORD [12+EBX],EDI
	MOV	DWORD [16+EBX],ESI
	CMP	DWORD [24+EBX],0
	JS	L294
	MOV	EAX,EDX
	ADD	EAX,DWORD [8+EBX]
	PUSH	EAX
	MOV	EAX,ECX
	ADD	EAX,DWORD [4+EBX]
	PUSH	EAX
	PUSH	EDX
	PUSH	ECX
	PUSH	DWORD [32+EBX]
	CALL	_sheetRefreshSub
	MOV	EAX,ESI
	ADD	EAX,DWORD [8+EBX]
	PUSH	EAX
	MOV	EAX,EDI
	ADD	EAX,DWORD [4+EBX]
	PUSH	EAX
	PUSH	ESI
	PUSH	EDI
	PUSH	DWORD [32+EBX]
	CALL	_sheetRefreshSub
	ADD	ESP,40
L294:
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
	GLOBAL	_sheetFree
_sheetFree:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EBX
	MOV	EBX,DWORD [8+EBP]
	CMP	DWORD [24+EBX],0
	JS	L297
	PUSH	-1
	PUSH	EBX
	CALL	_sheetUpdown
	POP	EAX
	POP	EDX
L297:
	MOV	DWORD [28+EBX],0
	MOV	EBX,DWORD [-4+EBP]
	LEAVE
	RET
[SECTION .data]
LC0:
	DB	"DickOS",0x00
LC1:
	DB	"(%d, %d)",0x00
LC2:
	DB	"memory %dMB free : %dKB",0x00
LC4:
	DB	"[lcr %4d %4d]",0x00
LC5:
	DB	"(%3d, %3d)",0x00
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
	SUB	ESP,500
	CALL	_initGdtit
	CALL	_init_pic
	CALL	_io_sti
	LEA	EAX,DWORD [-92+EBP]
	PUSH	EAX
	PUSH	32
	PUSH	_keybuf
	CALL	_QueueInit
	LEA	EAX,DWORD [-220+EBP]
	PUSH	EAX
	PUSH	128
	PUSH	_mousebuf
	CALL	_QueueInit
	PUSH	249
	PUSH	33
	CALL	_io_out8
	ADD	ESP,32
	PUSH	239
	PUSH	161
	CALL	_io_out8
	CALL	_initKeyboard
	LEA	EAX,DWORD [-236+EBP]
	PUSH	EAX
	CALL	_enableMouse
	PUSH	-1073741825
	PUSH	4194304
	CALL	_memtest
	PUSH	3932160
	MOV	EBX,EAX
	CALL	_memsegInit
	PUSH	647168
	PUSH	4096
	PUSH	3932160
	CALL	_memsegFree
	LEA	EAX,DWORD [-4194304+EBX]
	ADD	ESP,36
	PUSH	EAX
	PUSH	4194304
	PUSH	3932160
	CALL	_memsegFree
	CALL	_init_palette
	MOVSX	EAX,WORD [4086]
	PUSH	EAX
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [4088]
	PUSH	3932160
	CALL	_shtctlInit
	MOV	EBX,EAX
	PUSH	EAX
	CALL	_sheetAlloc
	ADD	ESP,32
	PUSH	EBX
	MOV	DWORD [-496+EBP],EAX
	LEA	EBX,DWORD [-492+EBP]
	CALL	_sheetAlloc
	MOVSX	EDX,WORD [4086]
	MOV	DWORD [-500+EBP],EAX
	MOVSX	EAX,WORD [4084]
	IMUL	EAX,EDX
	PUSH	EAX
	PUSH	3932160
	CALL	_memsegAlloc4K
	PUSH	-1
	MOV	DWORD [-504+EBP],EAX
	MOVSX	EAX,WORD [4086]
	PUSH	EAX
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [-504+EBP]
	PUSH	DWORD [-496+EBP]
	CALL	_sheetSetbuf
	ADD	ESP,32
	PUSH	14
	PUSH	16
	PUSH	16
	PUSH	EBX
	PUSH	DWORD [-500+EBP]
	CALL	_sheetSetbuf
	MOVSX	EAX,WORD [4086]
	PUSH	EAX
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [-504+EBP]
	CALL	_init_GUI
	ADD	ESP,32
	PUSH	14
	PUSH	EBX
	MOV	EBX,2
	CALL	_initMouseCursor8
	PUSH	0
	PUSH	0
	PUSH	DWORD [-496+EBP]
	CALL	_sheetSlide
	MOVSX	EAX,WORD [4084]
	LEA	ECX,DWORD [-16+EAX]
	MOV	EAX,ECX
	CDQ
	IDIV	EBX
	MOV	EDI,EAX
	MOVSX	EAX,WORD [4086]
	LEA	ECX,DWORD [-44+EAX]
	MOV	EAX,ECX
	CDQ
	IDIV	EBX
	PUSH	EAX
	MOV	ESI,EAX
	PUSH	EDI
	LEA	EBX,DWORD [-60+EBP]
	PUSH	DWORD [-500+EBP]
	CALL	_sheetSlide
	ADD	ESP,32
	PUSH	0
	PUSH	DWORD [-496+EBP]
	CALL	_sheetUpdown
	PUSH	1
	PUSH	DWORD [-500+EBP]
	CALL	_sheetUpdown
	PUSH	LC0
	PUSH	7
	PUSH	32
	PUSH	30
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [-504+EBP]
	CALL	_putFont8_asc
	ADD	ESP,40
	PUSH	ESI
	PUSH	EDI
	PUSH	LC1
	PUSH	EBX
	CALL	_sprintf
	PUSH	EBX
	PUSH	7
	PUSH	0
	PUSH	0
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [-504+EBP]
	CALL	_putFont8_asc
	ADD	ESP,40
	PUSH	3932160
	CALL	_memsegTotal
	SHR	EAX,10
	MOV	DWORD [ESP],EAX
	PUSH	-1073741825
	PUSH	4194304
	CALL	_memtest
	POP	EDX
	SHR	EAX,20
	POP	ECX
	PUSH	EAX
	PUSH	LC2
	PUSH	EBX
	CALL	_sprintf
	PUSH	EBX
	PUSH	7
	PUSH	64
	PUSH	0
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [-504+EBP]
	CALL	_putFont8_asc
	ADD	ESP,40
	PUSH	96
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	0
	PUSH	0
	PUSH	DWORD [-496+EBP]
	CALL	_sheetRefresh
	ADD	ESP,20
L299:
	CALL	_io_cli
	PUSH	_keybuf
	CALL	_QueueSize
	PUSH	_mousebuf
	MOV	EBX,EAX
	CALL	_QueueSize
	POP	EDX
	LEA	EAX,DWORD [EAX+EBX*1]
	POP	ECX
	TEST	EAX,EAX
	JE	L315
	PUSH	_keybuf
	CALL	_QueueSize
	POP	EBX
	TEST	EAX,EAX
	JNE	L316
	PUSH	_mousebuf
	CALL	_QueueSize
	POP	ECX
	TEST	EAX,EAX
	JE	L299
	PUSH	_mousebuf
	CALL	_QueuePop
	MOV	EBX,EAX
	CALL	_io_sti
	MOVZX	EAX,BL
	PUSH	EAX
	LEA	EAX,DWORD [-236+EBP]
	PUSH	EAX
	CALL	_mouseDecode
	ADD	ESP,12
	TEST	EAX,EAX
	JE	L299
	PUSH	DWORD [-228+EBP]
	PUSH	DWORD [-232+EBP]
	PUSH	LC4
	LEA	EBX,DWORD [-60+EBP]
	PUSH	EBX
	CALL	_sprintf
	ADD	ESP,16
	MOV	EAX,DWORD [-224+EBP]
	TEST	EAX,1
	JE	L308
	MOV	BYTE [-59+EBP],76
L308:
	TEST	EAX,2
	JE	L309
	MOV	BYTE [-57+EBP],82
L309:
	AND	EAX,4
	JE	L310
	MOV	BYTE [-58+EBP],67
L310:
	PUSH	31
	PUSH	151
	PUSH	16
	PUSH	32
	PUSH	14
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [-504+EBP]
	CALL	_boxFill8
	PUSH	EBX
	PUSH	7
	PUSH	16
	PUSH	32
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [-504+EBP]
	CALL	_putFont8_asc
	ADD	ESP,52
	PUSH	32
	PUSH	152
	PUSH	16
	PUSH	32
	PUSH	DWORD [-496+EBP]
	CALL	_sheetRefresh
	ADD	ESP,20
	ADD	ESI,DWORD [-228+EBP]
	ADD	EDI,DWORD [-232+EBP]
	JS	L317
L311:
	TEST	ESI,ESI
	JS	L318
L312:
	MOVSX	EAX,WORD [4084]
	DEC	EAX
	CMP	EDI,EAX
	JLE	L313
	MOV	EDI,EAX
L313:
	MOVSX	EAX,WORD [4086]
	DEC	EAX
	CMP	ESI,EAX
	JLE	L314
	MOV	ESI,EAX
L314:
	PUSH	ESI
	PUSH	EDI
	PUSH	LC5
	PUSH	EBX
	CALL	_sprintf
	PUSH	15
	PUSH	79
	PUSH	0
	PUSH	0
	PUSH	14
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [-504+EBP]
	CALL	_boxFill8
	ADD	ESP,44
	PUSH	EBX
	PUSH	7
	PUSH	0
	PUSH	0
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [-504+EBP]
	CALL	_putFont8_asc
	PUSH	16
	PUSH	80
	PUSH	0
	PUSH	0
	PUSH	DWORD [-496+EBP]
	CALL	_sheetRefresh
	ADD	ESP,44
	PUSH	ESI
	PUSH	EDI
	PUSH	DWORD [-500+EBP]
	CALL	_sheetSlide
	ADD	ESP,12
	JMP	L299
L318:
	XOR	ESI,ESI
	JMP	L312
L317:
	XOR	EDI,EDI
	JMP	L311
L316:
	PUSH	_keybuf
	CALL	_QueuePop
	MOV	EBX,EAX
	CALL	_io_sti
	PUSH	EBX
	LEA	EBX,DWORD [-60+EBP]
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
	PUSH	DWORD [-504+EBP]
	CALL	_boxFill8
	ADD	ESP,44
	PUSH	EBX
	PUSH	7
	PUSH	16
	PUSH	0
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [-504+EBP]
	CALL	_putFont8_asc
	PUSH	32
	PUSH	16
	PUSH	16
	PUSH	0
	PUSH	DWORD [-496+EBP]
	CALL	_sheetRefresh
	ADD	ESP,44
	JMP	L299
L315:
	CALL	_io_stihlt
	JMP	L299
	GLOBAL	_keybuf
[SECTION .data]
	ALIGNB	16
_keybuf:
	RESB	24
	GLOBAL	_mousebuf
[SECTION .data]
	ALIGNB	16
_mousebuf:
	RESB	24
