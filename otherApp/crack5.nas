[INSTRSET "i486p"]
[BITS 32]
		CALL	2*8:0x2ffe	; farCALL bootpack.map中记录的io_cli地址
		MOV		EDX,4
		INT		0x40
