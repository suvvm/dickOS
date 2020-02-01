; dickos-os boot asm
; TAB=4

[INSTRSET "i486p"]

VBEMODE 	EQU		0x105			; 画面模式 1024 * 768

BOTPAK		EQU		0x00280000		; 装载bootpack 
DSKCAC 		EQU		0x00100000		; 磁盘缓存位置
DSKCAC0		EQU		0x00008000		; 磁盘缓存位置（实时模式）

; BOOT_INFO
CYLS 		EQU		0x0ff0			; 启动区设置
LEDS		EQU		0x0ff1			; （不确定）键盘指示灯LED状态数
VMODE		EQU		0x0ff2			; 颜色数目信息 颜色的位数
SCRNX		EQU		0x0ff4			; 分辨率X（screen X）像素数
SCRNY 		EQU		0x0ff6			; 分辨率Y（screen Y）像素数
VRAM		EQU		0x0ff8			; 图像缓冲区开始地址

		ORG		0xc200			; 程序被装载的内存位置

; 确认是否支持VBE

		MOV		AX,0x9000	; 为AX赋值0x9000
		MOV		ES,AX		; 为ES赋值0x9000
		MOV		DI,0		; 为DI赋值0
		MOV		AX,0x4f00	; 为AX赋值0x4f00
		INT		0x10		; 发出中断信号 调用bios16号函数（16号中断处理函数调用显卡）
		CMP		AX,0x004f	; 比较AX与0x004f（有VBE AX会变为0x004f）
		JNE		scrn320		; VBE不存在就跳转 scrn320

; 检查VBE版本 VBE2.0以下版本不支持320 * 200以上的分辨率

		MOV		AX,[ES:DI+4]	; 获取VBE版本号
		CMP		AX,0x0200		; 与0x200比较
		JB		scrn320			; 版本号小于2.0 跳转至scrn320 

; 获取画面模式信息

		MOV		CX,VBEMODE		; CX赋值当前画面模式号
		MOV		AX,0x4f01		; AX赋值
		INT		0x10			; 发出中断信号 调用bios16号函数（16号中断处理函数调用显卡）
		CMP		AX,0x004f		; 比较AX与0x004f
		JNE		scrn320			; 不相等就跳转scrn320

; WORD	[ES:DI+0x00]	模式属性
; WORD	[ES:DI+0x12]	X分辨率
; WORD	[ES:DI+0x14]	Y分辨率
; BYTE	[ES:DI+0x19]	颜色数
; BYTE	[ES:DI+0x1b]	颜色指定方式 4为调色板模式
; DWORD	[ES:DI+0x28]	VRAM地址

; 确认画面模式信息

		CMP		BYTE [ES:DI+0x19],8		; 确认颜色数是否为8 不为8跳转scrn320
		JNE		scrn320					
		CMP		BYTE [ES:DI+0x1b],4		; 确认颜色的指定方式是否为调色板模式 不为4跳转scrn320
		JNE		scrn320
		MOV		AX,[ES:DI+0x00]
		AND		AX,0x0080
		JZ		scrn320					; 模式属性bit7为0 跳转至scrn320（不能+0x4000）

; 画面模式设定
; 使用VBE（VESA BIOS extension）时使用 AX = 0x4f02 BX = 画面模式号

		MOV		BX,VBEMODE+0x4000		; 向BX赋值画面模式号
		MOV		AX,0x4f02
		INT		0x10					; 发出中断信号 调用bios16号函数（16号中断处理函数调用显卡）
		MOV		BYTE [VMODE],8			; 记录画面模式
		MOV		AX,[ES:DI+0x12]
		MOV		[SCRNX],AX				; 设置X分辨率
		MOV		AX,[ES:DI+0x14]
		MOV		[SCRNY],AX				; 设置Y分辨率
		MOV		EAX,[ES:DI+0x28]
		MOV		[VRAM],EAX
		JMP		keystatus

; 不能使用VBE

scrn320:
		MOV		AL,0x13					; VGA模式 320 * 200 8bit
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000

;BIOS获得键盘LED指示灯状态

keystatus:
		MOV		AH,0x02
		INT		0x16			; 发出中断信号调用bios22号函数 （22号中断处理函数调用键盘）
		MOV		[LEDS],AL
		
; 使PCI不接受一切中断
; 根据AT兼容机的规格，初始化PIC（可编程中断控制器）
; 要在CLI（禁止CPU级别的中断）之前操作，否则有时会挂起

		MOV		AL,0xff
		OUT		0x21,AL
		NOP						; 如果继续执行OUT指令，有些机器可能无法正常运行，所以让CPU休息一个时钟周期
		OUT		0xa1,AL
		CLI						;CPU级别禁止中断
; 上述程序相当于
; io_out(PIC0_IMR, 0xff)禁止主PIC全部中断
; io_out(PIC1_IMR, 0xff)禁止从PIC全部中断
; io_cli()禁止cpu级别的中断

		
; 启用A20GATE，使CPU能够访问1MB以上的存储器
; waitkbdout: 等同于keyboard.c中定义的函数waitKeyboardControllerReady()
		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; enable A20
		OUT		0x60,AL
		CALL	waitkbdout
; 上述程序等同于：
;	#define KEYCMD_WRITE_OUTPOT	0xd1
;	#define KBC_OUTPORT_A20G_ENABLE	0xdf
;	waitKeyboardControllerReady()
;	io_out8(PORT_KEYCMD, KEYCMD_WRITE_OUTPOT)
;	waitKeyboardControllerReady()
;	io_out8(PORT_KEYDAT, KBC_OUTPORT_A20G_ENABLE)	激活A20GATE信号线 使1MB以上内存可用
;	waitKeyboardControllerReady() 等待指令执行完成 并无实际用处





; 保护模式转换
; 保护模式：应用程序不能随意改变段设置 也不能使用操作系统专用段 操作系统收到CPU保护

[INSTRSET "i486p"]				; 要使用i486命令的语句（LGDT EAX CR0等）

		LGDT	[GDTR0]			; 设置临时GDT
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	; 将bit31设置为0（为了禁止分页）
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

; bootpack引导程序包传输 相当于C语音中使用memcpy 传送一段内存地址中的内容到另一段内存地址中

		MOV		ESI,bootpack	; 传输源
		MOV		EDI,BOTPAK		; 目的地
		MOV		ECX,512*1024/4
		CALL		memcpy
; memcpy(bootpack, BOTPAK, 512*1024/4)
; 将bootpack.hrb 复制到0x00280000号地址 512KB的原因是多了总比少了好

; 此外，磁盘数据将传输到原始位置

; 从启动扇区开始

		MOV		ESI,0x7c00		; 传输源
		MOV		EDI,DSKCAC		; 目的地
		MOV		ECX,512/4
		CALL		memcpy
; memcpy(0x7c00, DSKCAC, 512/4) 从0x7c00复制512字节到0x00100000（将启动区复制到1MB以后的内存中）
		
; 剩余全部

		MOV		ESI,DSKCAC0+512	; 传输源
		MOV		EDI,DSKCAC+512	; 目的地
		MOV		ECX,0
		MOV		CL,BYTE [CYLS]
		IMUL		ECX,512*18*2/4	; 将柱面数转换为字节数/4
		SUB		ECX,512/4		; 减去IPL
		CALL		memcpy
; memcpy(DSKCAC0+512, DSKCAC+512, CYLS*512*18*2/4 - 512/4)
; 将 0x00008200复制到0x00100200中去 由于传送数据大小是以柱面数来计算的，所以需要减去启动区长度


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
		CALL		memcpy
		
skip:
		MOV		ESP,[EBX+12]	; 堆栈初始值
		JMP		DWORD 2*8:0x0000001b
; memcpy(0x10c8, 0x00310000, 0x11a8) 
	
	
; 等待键盘控制电路就绪，如果其中有键盘或鼠标数据就将其读出	
waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02		; 清空缓冲区中的垃圾数据
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
	
	
		ALIGNB		16			; 一直添加DBO直到地址能被16整除（为了保证MOV指令速度）
GDT0:
		RESB		8				; 空选择器
		DW		0xffff,0x0000,0x9200,0x00cf	; I/O段32bit
		DW		0xffff,0x0000,0x9a28,0x0047	; 可执行段32bit（用于bootpack）
		DW		0
GDTR0:
		DW		8*3-1
		DD		GDT0

		ALIGNB		16
bootpack:
