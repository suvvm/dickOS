; dickos-os
; TAB=4

; BOOT_INFO
CYLS 	EQU		0x0ff0			; 启动区设置
LEDS	EQU		0x0ff1			; （不确定）键盘指示灯LED状态数
VMODE	EQU		0x0ff2			; 颜色数目信息 颜色的位数
SCRNX	EQU		0x0ff3			; 分辨率X（screen X）像素数
SCRNY 	EQU 	0x0ff4			; 分辨率Y（screen Y）像素数
VRAM	EQU 	0x0ff5			; 图像缓冲区开始地址
		
		ORG		0x200			; 程序被装载的内存位置
		MOV		AL,0x13
		MOV		AH,0x00
		INT		0x10			; 调用bios16号函数（调用显卡）
		MOV 	BYTE [VMODE],8	; 记录画面模式
		MOV 	WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV  	DWORD [VRAM],0x000a0000
		
;BIOS获得键盘LED指示灯状态

		MOV 	AH,0x02
		INT		0x16			; 调用bios22号函数 （调用键盘）
		MOV 	[LEDS],AL
		
fin:
		HLT
		JMP		fin