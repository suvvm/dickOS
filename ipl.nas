; hello-os
; TAB=4

		ORG		0x7c00			; 指明程序的装载地址(启动区内容的装载地址0x00007c00-0x00007dff)

; FAT12软盘格式

		JMP		entry
		DB		0x90
		DB		"DICKSIPL"		; 启动分区名称
		DW		512				; 扇区(sector)大小锁定512字节
		DB		1				; 簇(cluster)大小锁定一个扇区
		DW		1				; FAT的起始位置（从第一个扇区开始）
		DB		2				; FAT的个数锁定2
		DW		224				; 根目录大小(224项)
		DW		2880			; 磁盘大小(按软盘标准算必定为2880扇区)
		DB		0xf0			; 磁盘种类锁定0xf0
		DW		9				; FAT长度锁定9扇区
		DW		18				; 1个磁道(track)有几个扇区锁定18
		DW		2				; 磁头数(锁定2)
		DD		0				; 不使用分区锁定0
		DD		2880			; 重写磁盘大小
		DB		0,0,0x29		; 不知道什么意思
		DD		0xffffffff		; (可能)卷标号码
		DB		"DICK-OS    "	; 磁盘名称(11字节)
		DB		"FAT12   "		; 磁盘格式(8字节)
		RESB	18				; 空出18字节

; 程序主体

entry:
		MOV		AX,0			; 初始化寄存器
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX
		
;添加部分

		MOV		AX,0x0820
		MOV 	ES,AX
		MOV		CH,0			; 柱面0
		MOV		DH,0			; 磁头0
		MOV		CL,2			; 扇区2
		
		MOV 	AH,0x02			; AH=0x02 : 读盘
		MOV		AL,1			; 一个扇区
		MOV		BX,0			
		MOV		DL,0x00			; A驱动器
		INT		0x13			; 调用bios 19号函数（磁盘操作 0x02:读盘）
		JC		error
		
		
		
fin:
		HLT						; 让cpu停止等待指令
		JMP		fin				; 无限循环
		
error:
		MOV 	SI,msg
putloop:
		MOV		AL,[SI]
		ADD		SI,1			; SI加1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; 显示一个文字
		MOV		BX,15			; 指定字符颜色
		INT		0x10			; 调用bios 16号函数（调用显卡）
		JMP		putloop
msg:
		DB		0x0a, 0x0a		; 两个换行符
		DB		"load error"
		DB		0x0a			; 换行
		DB		0

		RESB	0x7dfe-$		; 填写0x00直到0x001fe

		DB		0x55, 0xaa
