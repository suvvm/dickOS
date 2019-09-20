;dickos
;TAB=4

;FAT12软盘格式
;DB define byte像文件中写入1个字节的汇编指令
;DW define word 写入2个字节(word为16位2个字节)
;DD define double-word 写入4个字节
	DB 	0xeb,0x4e,0x90
	DB 	"DICKIPL"	;启动分区名称
	DW 	512			;扇区(sector)大小锁定512字节
	DB 	1			;簇(cluster)大小锁定一个扇区
	DW 	1			;FAT的起始位置（从第一个扇区开始）
	DB 	2			;FAT的个数锁定2
	DW	224			;根目录大小(224项)
	DW 	2880		;磁盘大小(按软盘标准算必定为2880扇区)
	DB	0xf0		;磁盘种类锁定0xf0
	DW	9			;FAT长度锁定9扇区
	DW	18			;1个磁道(track)有几个扇区锁定18
	DW	2			;磁头数(锁定2)
	DD	0			;不使用分区锁定0
	DD	2880		;重写磁盘大小
	DB	0,0,0x29	;不知道什么意思
	DD	0xffffffff	;(可能)卷标号码
	DB 	"DICK-OS	"	;磁盘名称(11字节)
	DB  "FAT12	"		;磁盘格式(8字节)
	RESB 18				;空出18字节