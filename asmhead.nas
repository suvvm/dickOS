; dickos-os boot asm
; TAB=4

BOTPAK	EQU		0x00280000		;装载bootpack 
DSKCAC 	EQU		0x00100000		;磁盘缓存位置
DSKCAC0	EQU		0x00008000		;磁盘缓存位置（实时模式）

; BOOT_INFO
CYLS 	EQU		0x0ff0			; 启动区设置
LEDS	EQU		0x0ff1			; （不确定）键盘指示灯LED状态数
VMODE	EQU		0x0ff2			; 颜色数目信息 颜色的位数
SCRNX	EQU		0x0ff4			; 分辨率X（screen X）像素数
SCRNY 	EQU 	0x0ff6			; 分辨率Y（screen Y）像素数
VRAM	EQU 	0x0ff8			; 图像缓冲区开始地址
		
		ORG		0xc200			; 程序被装载的内存位置

; 画面模式设定

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
		
; 使PCI不接受一切中断

; 根据AT兼容机的规格，初始化PIC

		MOV		AL,0xff
		OUT		0x21,AL
		NOP						; 如果继续执行OUT指令，似乎有一个模块不起作用
		OUT		0xa1,AL
		
		CLI						;CPU级别，也禁止中断
		
; 启用A20GATE，使CPU能够访问1MB以上的存储器

		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; enable A20
		OUT		0x60,AL
		CALL	waitkbdout

; 保护模式转换

[INSTRSET "i486p"]				; 要使用i486命令的语句

		LGDT	[GDTR0]			; 设置临时GDT
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	; 将bit31设置为0（因为禁止分页）
		OR		EAX,0x00000001	; 将bit0设置为1（切换到保护模式）
		MOV		CR0,EAX
		JMP		pipelineflush
pipelineflush:
		MOV		AX,1*8			; I/O段32bit
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX

; bootpack引导程序包传输

		MOV		ESI,bootpack	; 传输源
		MOV		EDI,BOTPAK		; 目的地
		MOV		ECX,512*1024/4
		CALL	memcpy

; 此外，磁盘数据将传输到原始位置

; 从引导扇区开始

		MOV		ESI,0x7c00		; 传输源
		MOV		EDI,DSKCAC		; 目的地
		MOV		ECX,512/4
		CALL	memcpy
		
; 剩余全部

		MOV		ESI,DSKCAC0+512	; 传输源
		MOV		EDI,DSKCAC+512	; 目的地
		MOV		ECX,0
		MOV		CL,BYTE [CYLS]
		IMUL	ECX,512*18*2/4	; 将柱面数转换为字节数/4
		SUB		ECX,512/4		; 减去IPL
		CALL	memcpy

; asmhead功能完成
; 将其放到bootpack中

; bootpack启动

		MOV		EBX,BOTPAK
		MOV		ECX,[EBX+16]
		ADD		ECX,3			; ECX += 3
		SHR		ECX,2			; ECX /= 4
		JZ		skip			; 没有可以转移的
		MOV		ESI,[EBX+20]	; 传输源
		ADD		ESI,EBX
		MOV		EDI,[EBX+12]	; 目的地
		CALL	memcpy
		
skip:
		MOV		ESP,[EBX+12]	; 堆栈初始值
		JMP		DWORD 2*8:0x0000001b
		
waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02
		JNZ		waitkbdout		; 如果AND结果不为0，跳转到waitkbdout
		RET
memcpy:
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy			; 如果减法的结果不为0，跳转到memcpy
		RET
		ALIGNB	16
GDT0:
		RESB	8				; 空选择器
		DW		0xffff,0x0000,0x9200,0x00cf	; I/O段32bit
		DW		0xffff,0x0000,0x9a28,0x0047	; 可执行段32bit（用于bootpack）
		DW		0
GDTR0:
		DW		8*3-1
		DD		GDT0

		ALIGNB	16
bootpack:
