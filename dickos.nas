; dickos-os
; TAB=4

		ORG		0x200			; 程序被装载的内存位置
		
		MOV		AL,0x13
		MOV		AH,0x00
		INT		0x10			; 调用bios16号函数（调用显卡）
fin:
		HLT
		JMP		fin