[BITS 32]
		MOV		AL,'h'
		CALL	2*8:0x2f6d	; far-CALL需要指定段和偏移量 系统调用 打印h
		MOV		AL,'e'
		CALL	2*8:0x2f6d	; 系统调用 打印e
		MOV		AL,'l'
		CALL	2*8:0x2f6d	; 系统调用 打印l
		MOV		AL,'l'
		CALL	2*8:0x2f6d	; 系统调用 打印l
		MOV		AL,'o'
		CALL	2*8:0x2f6d	; 系统调用 打印o
		RETF
