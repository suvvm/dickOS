[INSTRSET "i486p"]
[BITS 32]
		MOV		ECX,msg
putloop:
		MOV		AL,[CS:ECX]
		CMP		AL,0
		JE		fin
		INT		0x40		; INT中断方式调用函数asm_consolePutchar
		ADD		ECX,1
		JMP		putloop

fin:
		RETF

msg:
		DB "hello",0
