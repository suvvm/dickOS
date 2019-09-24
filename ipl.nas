; dickos-os
; TAB=4

		ORG		0x7c00			; 指明程序的装?地址(??区内容的装?地址0x00007c00-0x00007dff)

; FAT12??格式

		JMP		entry
		DB		0x90
		DB		"DICKSIPL"		; ??分区名称（8字?）
		DW		512				; 扇区(sector)大小?定512字?
		DB		1				; 簇(cluster)大小?定一个扇区
		DW		1				; FAT的起始位置（从第一个扇区?始）
		DB		2				; FAT的个数?定2
		DW		224				; 根目?大小(224?)
		DW		2880			; 磁?大小（按???准算必定?2880扇区）
		DB		0xf0			; 磁????定0xf0
		DW		9				; FAT?度?定9扇区
		DW		18				; 1个磁道(track)有几个扇区?定18
		DW		2				; 磁?数(?定2)
		DD		0				; 不使用分区?定0
		DD		2880			; 重写磁?大小
		DB		0,0,0x29		; 不知道什?意思
		DD		0xffffffff		; (可能)卷?号?
		DB		"DICK-OS    "	; 磁?名称(11字?)
		DB		"FAT12   "		; 磁?格式(8字?)
		RESB	18				; 空出18字?

; 程序主体

entry:
		MOV		AX,0			; 初始化寄存器
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX

; 本次添加

		MOV		AX,0x0820
		MOV		ES,AX
		MOV		CH,0			; 柱面0
		MOV		DH,0			; 磁?0
		MOV		CL,2			; 扇区2
readloop:
		MOV		SI,0			; ??失?次数的寄存器
retry:
		MOV		AH,0x02			; AH=0x02 : ??
		MOV		AL,1			; 一个扇区
		MOV		BX,0
		MOV		DL,0x00			; A??器
		INT		0x13			; ?用bios 19号函数（磁?操作 0x02:??）
		JNC		next			; 没有出??跳?到next，?下一个扇区
		ADD		SI,1			; 出?次数SI加一
		CMP		SI,5			; SI与5比?
		JAE		error			; SI >= 5 跳?至error
		MOV		AH,0x00			; AH??0x00
		MOV		DL,0x00			; A??器
		INT		0x13			; ?用bios 19号函数（磁?操作 0x00:重置??器）
		JMP		retry
next:
		MOV		AX,ES			; 内存地址后移0x200
		ADD		AX,0x0020		; AX一个扇区512字?512的16?制?20，?下一个扇区只需在原地址加0x20即可
		MOV		ES,AX			; 由于不能直接?ES?行ADD指令，所以利用AX?ES??
		ADD		CL,1			; CL加一
		CMP		CL,18			; 比?CL与18
		JBE		readloop		; 如果CL<=18跳?至readloop

fin:
		HLT						; ?cpu停止等待指令
		JMP		fin				; 无限循?

error:
		MOV		SI,msg
putloop:
		MOV		AL,[SI]
		ADD		SI,1			; SI加1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; ?示一个文字
		MOV		BX,15			; 指定字符?色
		INT		0x10			; ?用bios 16号函数（?用??）
		JMP		putloop
msg:
		DB		0x0a, 0x0a		; ?个?行符
		DB		"load error"
		DB		0x0a			; ?行
		DB		0

		RESB	0x7dfe-$		; 填写0x00直到0x001fe

		DB		0x55, 0xaa
