; func
; TAB=4

[FORMAT "WCOFF"]				; 制作目标文件的模式
[INSTRSET "i486p"]				
[BITS 32]						; 制作32位模式

; 制作目标文件的信息

[FILE "func.nas"]				; 源文件名
		GLOBAL _io_hlt, _io_cli, _io_sti, _io_stihlt	; 程序中包含的函数名
		GLOBAL _io_in8, _io_in16, _io_in32
		GLOBAL _io_out8, _io_out16, _io_out32
		GLOBAL _io_load_eflags, _io_store_eflags
		GLOBAL _loadGdtr, _loadIdtr
		GLOBAL	_asm_interruptHandler21, _asm_interruptHandler27, _asm_interruptHandler2c
		EXTERN	_interruptHandler21, _interruptHandler27, _interruptHandler2c
; 实际的函数

[SECTION .text]					; 目标文件中写了这些后再写程序

_io_hlt:						; void io_hlt();
		HLT
		RET

_io_cli:						; void io_cli();
		CLI
		RET

_io_sti:						; void io_sti();
		STI
		RET
		
_io_stihlt:						; void io_stihlt();
		STI
		HLT
		RET
		
_io_in8:						; void io_in8(int port);
		MOV		EDX,[ESP+4]		; 端口
		MOV		EAX,0
		IN		AL,DX			; 从DX端口读取一字节数据到AL
		RET
		
_io_in16:						; void io_in16(int port);
		MOV		EDX,[ESP+4]		; 端口
		MOV		EAX,0
		IN		AX,DX			; 从DX端口读取一字节数据到AX
		RET
		
_io_in32:						; void io_in8(int port);
		MOV		EDX,[ESP+4]		; 端口
		IN		EAX,DX			; 从DX端口读取一字节数据到EAX
		RET

_io_out8:						; void io_out8(int port, int data);
		MOV		EDX,[ESP+4]		; 端口port
		MOV		AL,[ESP+8]		; 数据data
		OUT		DX,AL			; 将AL值写入端口
		RET
		
_io_out16:						; void io_out16(int port, int data);
		MOV 	EDX,[ESP+4]		; 端口port
		MOV		EAX,[ESP+8]		; 数据data
		OUT		DX,AX			; 将AL值写入端口
		RET
		
_io_out32:						; void io_out32(int port, int data);
		MOV 	EDX,[ESP+4]		; 端口port
		MOV		EAX,[ESP+8]		; 数据data
		OUT		DX,EAX			; 将AL值写入端口
		RET
		
_io_load_eflags:				; int io_load_eflags(void);
		PUSHFD					; PUSH EFLAGS
		POP		EAX
		RET

_io_store_eflags:				; void io_store_eflags(int eflags);
		MOV		EAX,[ESP+4]
		PUSH		EAX
		POPFD					; POP EFLAGS
		RET
		
_loadGdtr:						; void loadGdtr(int limit, int addr); 将指定段的上限和地址赋值给寄存器GDTR
		MOV		AX,[ESP+4]
		MOV		[ESP+6],AX
		LGDT	[ESP+6]
		RET
		
_loadIdtr:						; void loadIdtr(int limit, int  addr);
		MOV		AX,[ESP+4]
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET

_asm_interruptHandler21:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_interruptHandler21
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_interruptHandler27:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_interruptHandler27
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_interruptHandler2c:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_interruptHandler2c
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD
