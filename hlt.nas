[BITS 32]
		MOV		AL,'h'
		INT		0x40		; INT中断方式调用函数asm_consolePutchar 系统调用 打印h
		MOV		AL,'e'
		INT		0x40	; 系统调用 打印e
		MOV		AL,'l'
		INT		0x40	; 系统调用 打印l
		MOV		AL,'l'
		INT		0x40	; 系统调用 打印l
		MOV		AL,'o'
		INT		0x40	; 系统调用 打印o
		RETF
