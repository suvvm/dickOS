; hello-os
; TAB=4

		ORG		0x7c00			; ָ�������װ�ص�ַ(���������ݵ�װ�ص�ַ0x00007c00-0x00007dff)

; FAT12���̸�ʽ

		JMP		entry
		DB		0x90
		DB		"DICKSIPL"		; ������������
		DW		512				; ����(sector)��С����512�ֽ�
		DB		1				; ��(cluster)��С����һ������
		DW		1				; FAT����ʼλ�ã��ӵ�һ��������ʼ��
		DB		2				; FAT�ĸ�������2
		DW		224				; ��Ŀ¼��С(224��)
		DW		2880			; ���̴�С(�����̱�׼��ض�Ϊ2880����)
		DB		0xf0			; ������������0xf0
		DW		9				; FAT��������9����
		DW		18				; 1���ŵ�(track)�м�����������18
		DW		2				; ��ͷ��(����2)
		DD		0				; ��ʹ�÷�������0
		DD		2880			; ��д���̴�С
		DB		0,0,0x29		; ��֪��ʲô��˼
		DD		0xffffffff		; (����)������
		DB		"DICK-OS    "	; ��������(11�ֽ�)
		DB		"FAT12   "		; ���̸�ʽ(8�ֽ�)
		RESB	18				; �ճ�18�ֽ�

; ��������

entry:
		MOV		AX,0			; ��ʼ���Ĵ���
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX
		
;��Ӳ���

		MOV		AX,0x0820
		MOV 	ES,AX
		MOV		CH,0			; ����0
		MOV		DH,0			; ��ͷ0
		MOV		CL,2			; ����2
		
		MOV 	AH,0x02			; AH=0x02 : ����
		MOV		AL,1			; һ������
		MOV		BX,0			
		MOV		DL,0x00			; A������
		INT		0x13			; ����bios 19�ź��������̲��� 0x02:���̣�
		JC		error
		
		
		
fin:
		HLT						; ��cpuֹͣ�ȴ�ָ��
		JMP		fin				; ����ѭ��
		
error:
		MOV 	SI,msg
putloop:
		MOV		AL,[SI]
		ADD		SI,1			; SI��1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; ��ʾһ������
		MOV		BX,15			; ָ���ַ���ɫ
		INT		0x10			; ����bios 16�ź����������Կ���
		JMP		putloop
msg:
		DB		0x0a, 0x0a		; �������з�
		DB		"load error"
		DB		0x0a			; ����
		DB		0

		RESB	0x7dfe-$		; ��д0x00ֱ��0x001fe

		DB		0x55, 0xaa
