; dickos-os
; TAB=4

		ORG		0x7c00			; w¾öI?n¬(??æàeI?n¬0x00007c00-0x00007dff)

; FAT12??i®

		JMP		entry
		DB		0x90
		DB		"DICKSIPL"		; ??ªæ¼Ìi8?j
		DW		512				; îæ(sector)å¬?è512?
		DB		1				; âÆ(cluster)å¬?èê¢îæ
		DW		1				; FATINnÊui¸æê¢îæ?nj
		DB		2				; FATI¢?è2
		DW		224				; ªÚ?å¬(224?)
		DW		2880			; ¥?å¬iÂ???yZKè?2880îæj
		DB		0xf0			; ¥????è0xf0
		DW		9				; FAT?x?è9îæ
		DW		18				; 1¢¥¹(track)L{¢îæ?è18
		DW		2				; ¥?(?è2)
		DD		0				; sgpªæ?è0
		DD		2880			; dÊ¥?å¬
		DB		0,0,0x29		; sm¹Y?Óv
		DD		0xffffffff		; (Â\)É??
		DB		"DICK-OS    "	; ¥?¼Ì(11?)
		DB		"FAT12   "		; ¥?i®(8?)
		RESB	18				; óo18?

; öåÌ

entry:
		MOV		AX,0			; n»ñ¶í
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX

; {YÁ

		MOV		AX,0x0820
		MOV		ES,AX
		MOV		CH,0			; Ê0
		MOV		DH,0			; ¥?0
		MOV		CL,2			; îæ2
readloop:
		MOV		SI,0			; ??¸?Iñ¶í
retry:
		MOV		AH,0x02			; AH=0x02 : ??
		MOV		AL,1			; ê¢îæ
		MOV		BX,0
		MOV		DL,0x00			; A??í
		INT		0x13			; ?pbios 19i¥?ì 0x02:??j
		JNC		next			; vLo??µ?nextC?ºê¢îæ
		ADD		SI,1			; o?SIÁê
		CMP		SI,5			; SI^5ä?
		JAE		error			; SI >= 5 µ?error
		MOV		AH,0x00			; AH??0x00
		MOV		DL,0x00			; A??í
		INT		0x13			; ?pbios 19i¥?ì 0x00:du??íj
		JMP		retry
next:
		MOV		AX,ES			; à¶n¬@Ú0x200
		ADD		AX,0x0020		; AXê¢îæ512?512I16?§?20C?ºê¢îæüùÝ´n¬Á0x20¦Â
		MOV		ES,AX			; R°s\¼Ú?ES?sADDwßCÈpAX?ES??
		ADD		CL,1			; CLÁê
		CMP		CL,18			; ä?CL^18
		JBE		readloop		; @ÊCL<=18µ?readloop

fin:
		HLT						; ?cpuâ~Òwß
		JMP		fin				; ÙÀz?

error:
		MOV		SI,msg
putloop:
		MOV		AL,[SI]
		ADD		SI,1			; SIÁ1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; ?¦ê¢¶
		MOV		BX,15			; wè?F
		INT		0x10			; ?pbios 16i?p??j
		JMP		putloop
msg:
		DB		0x0a, 0x0a		; ?¢?s
		DB		"load error"
		DB		0x0a			; ?s
		DB		0

		RESB	0x7dfe-$		; UÊ0x00¼0x001fe

		DB		0x55, 0xaa
