; dickos-os
; TAB=4

		ORG		0x7c00			; �w�������I��?�n��(??����e�I��?�n��0x00007c00-0x00007dff)

; FAT12??�i��

		JMP		entry
		DB		0x90
		DB		"DICKSIPL"		; ??���於�́i8��?�j
		DW		512				; ���(sector)�召?��512��?
		DB		1				; ��(cluster)�召?��꘢���
		DW		1				; FAT�I�N�n�ʒu�i����꘢���?�n�j
		DB		2				; FAT�I����?��2
		DW		224				; ����?�召(224?)
		DW		2880			; ��?�召�i��???�y�Z�K��?2880���j
		DB		0xf0			; ��????��0xf0
		DW		9				; FAT?�x?��9���
		DW		18				; 1������(track)�L�{�����?��18
		DW		2				; ��?��(?��2)
		DD		0				; �s�g�p����?��0
		DD		2880			; �d�ʎ�?�召
		DB		0,0,0x29		; �s�m���Y?�ӎv
		DD		0xffffffff		; (�\)��?��?
		DB		"DICK-OS    "	; ��?����(11��?)
		DB		"FAT12   "		; ��?�i��(8��?)
		RESB	18				; ��o18��?

; �������

entry:
		MOV		AX,0			; ���n���񑶊�
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX

; �{���Y��

		MOV		AX,0x0820
		MOV		ES,AX
		MOV		CH,0			; ����0
		MOV		DH,0			; ��?0
		MOV		CL,2			; ���2
readloop:
		MOV		SI,0			; ??��?�����I�񑶊�
retry:
		MOV		AH,0x02			; AH=0x02 : ??
		MOV		AL,1			; �꘢���
		MOV		BX,0
		MOV		DL,0x00			; A??��
		INT		0x13			; ?�pbios 19�������i��?���� 0x02:??�j
		JNC		next			; �v�L�o??��?��next�C?���꘢���
		ADD		SI,1			; �o?����SI����
		CMP		SI,5			; SI�^5��?
		JAE		error			; SI >= 5 ��?��error
		MOV		AH,0x00			; AH??0x00
		MOV		DL,0x00			; A??��
		INT		0x13			; ?�pbios 19�������i��?���� 0x00:�d�u??��j
		JMP		retry
next:
		MOV		AX,ES			; �����n���@��0x200
		ADD		AX,0x0020		; AX�꘢���512��?512�I16?��?20�C?���꘢�������݌��n����0x20����
		MOV		ES,AX			; �R���s�\����?ES?�sADD�w�߁C���ȗ��pAX?ES??
		ADD		CL,1			; CL����
		CMP		CL,18			; ��?CL�^18
		JBE		readloop		; �@��CL<=18��?��readloop

fin:
		HLT						; ?cpu��~���Ҏw��
		JMP		fin				; �ٌ��z?

error:
		MOV		SI,msg
putloop:
		MOV		AL,[SI]
		ADD		SI,1			; SI��1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; ?���꘢����
		MOV		BX,15			; �w�莚��?�F
		INT		0x10			; ?�pbios 16�������i?�p??�j
		JMP		putloop
msg:
		DB		0x0a, 0x0a		; ?��?�s��
		DB		"load error"
		DB		0x0a			; ?�s
		DB		0

		RESB	0x7dfe-$		; �U��0x00����0x001fe

		DB		0x55, 0xaa
