[INSTRSET "i486p"]
[BITS 32]
		CLI

; 应用程序模式运行时执行CLI STI HLT都会产生异常

fin:
		HLT
		JMP		fin
