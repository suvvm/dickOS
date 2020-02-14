[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "crack7.nas"]

		GLOBAL	_Main
	
[SECTION .text]

_Main:
		MOV		AX,1005*8
		MOV		DS,AX						;攻击段号1005（第一个命令行窗口的应用程序代码段）
		CMP		DWORD [DS:0x0004], 'Hari'	; 判断是否为应用程序
		JNE		fin							; 不是应用程序不进行攻击
		
		MOV		ECX,[DS:0x0000]				; 读取应用程序数据段的大小
		MOV		AX, 2005*8
		MOV		DS,AX						; 切换到段号2005
		
crackloop:									; 循环将其中的所有内容替换为无效内容123
		ADD		ECX,-1
		MOV		BYTE [DS:ECX], 123
		CMP		ECX,0
		JNE		crackloop

fin:
		MOV		EDX,4
		INT		0x40
