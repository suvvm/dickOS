[BITS 32]
	MOV		AL,'A'
	CALL	2*8:0x2f6d	; far-CALL需要指定段和偏移量
fin:
	HLT
	JMP 	fin