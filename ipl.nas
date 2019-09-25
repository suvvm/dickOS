; dickos-os
; TAB=4

;本次添加
CYLS 	EQU 	10				; 读取柱面（cylinders）数量为10

		ORG		0x7c00			; 指明程序的装载d地址(启动区内容的装载地址0x00007c00-0x00007dff)

; FAT12格式代码

		JMP		entry
		DB		0x90
		DB		"DICKSIPL"		; 分区名称（8字节）
		DW		512				; 扇区（sector）（大小512字节）
		DB		1				; 簇(cluster)（大小1个扇区）
		DW		1				; FAT的起始位置（从第一个扇区开始）
		DB		2				; FAT的个数（必须为2）
		DW		224				; 根目录的大小(224项)
		DW		2880			; 磁盘大小（必须为2880扇区）
		DB		0xf0			; 磁盘的种类（必须为0xf0）
		DW		9				; FAT的长度（9扇区）
		DW		18				; 1个磁道（track）有几个扇区（必须为18）
		DW		2				; 磁头数（必须为2）
		DD		0				; 不使用分区（必须为0）
		DD		2880			; 重写硬盘大小
		DB		0,0,0x29		; 不知道什么意思
		DD		0xffffffff		; （可能）卷标
		DB		"DICK-OS    "	; 磁盘名称（11字节）
		DB		"FAT12   "		; 磁盘格式名称（8字节）
		RESB	18				; 空出18字节

; 程序主题

entry:
		MOV		AX,0			; 初始化寄存器
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX
		MOV		AX,0x0820
		MOV		ES,AX
		MOV		CH,0			; 柱面0
		MOV		DH,0			; 磁头0
		MOV		CL,2			; 扇区2
readloop:
		MOV		SI,0			; 记录失败次数的寄存器
retry:
		MOV		AH,0x02			; AH=0x02 : 读入磁盘
		MOV		AL,1			; 一个扇区
		MOV		BX,0
		MOV		DL,0x00			; A驱动器
		INT		0x13			; 使用bios19号函数（磁盘操作 0x02:读盘）
		JNC		next			; 没有出现错误跳转到next，读取下一个扇区
		ADD		SI,1			; 出错次数SI加一
		CMP		SI,5			; SI与5比较
		JAE		error			; SI >= 5 跳转至error
		MOV		AH,0x00			; AH赋值0x00
		MOV		DL,0x00			; A驱动器
		INT		0x13			; 使用bios19号函数（磁盘操作 0x00:重置驱动器）
		JMP		retry
next:
		MOV		AX,ES			; 内存地址后移0x200
		ADD		AX,0x0020		; AX一个扇区512字节，512的16进制为0x20，读取下一个扇区只需在原地址加0x20即可
		MOV		ES,AX			; 由于不能直接对ES执行ADD指令，所以利用AX对EX进行赋值
		ADD		CL,1			; CL加一
		CMP		CL,18			; 比较CL与18
		
; 本次添加

		JBE		readloop		; 如果CL<=18跳转至readloop
		MOV		CL,1			; 扇区CL重置为1
		ADD		DH,1			; 磁头DH为1
		CMP		DH,2			; 比较DH与2
		JB 		readloop		; DH<2时跳转至readloop
		MOV 	DH,0			; 磁头DH重置为1
		ADD		CH,1			; 柱面CH加一
		CMP		CH,CYLS			; 比较柱面CH与CYLS
		JB		readloop		; 柱面CH小于10跳转至readloop

fin:
		HLT						; cpu停止等待指令
		JMP		fin				; 无限循环

error:
		MOV		SI,msg
putloop:
		MOV		AL,[SI]
		ADD		SI,1			; SI加1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; 显示一个文字
		MOV		BX,15			; 指定字符颜色
		INT		0x10			; 调用bios16号函数（调用显卡）
		JMP		putloop
msg:
		DB		0x0a, 0x0a		; 两个换行符
		DB		"load error"
		DB		0x0a			; 换行符
		DB		0

		RESB	0x7dfe-$		; 填写0x00直到0x001fe

		DB		0x55, 0xaa
