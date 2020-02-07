; func
; TAB=4

[FORMAT "WCOFF"]				; 制作目标文件的模式
[INSTRSET "i486p"]				
[BITS 32]						; 制作32位模式

; 制作目标文件的信息

[FILE "func.nas"]				; 源文件名
		GLOBAL	_io_hlt, _io_cli, _io_sti, _io_stihlt	; 程序中包含的函数名
		GLOBAL	_io_in8, _io_in16, _io_in32
		GLOBAL	_io_out8, _io_out16, _io_out32
		GLOBAL	_io_load_eflags, _io_store_eflags
		GLOBAL	_loadGdtr, _loadIdtr
		GLOBAL	_loadCr0, _storeCr0
		GLOBAL	_loadTr
		GLOBAL	_asm_interruptHandler20, _asm_interruptHandler21, _asm_interruptHandler27, _asm_interruptHandler2c
		GLOBAL	_memtest_sub
		GLOBAL	_farJmp, _farCall
		GLOBAL	_asm_consolePutchar
		EXTERN	_interruptHandler20, _interruptHandler21, _interruptHandler27, _interruptHandler2c
		EXTERN	_consolePutchar
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
		
_io_load_eflags:				; int io_load_eflags(void); 读取eflags
		PUSHFD					; PUSH EFLAGS
		POP		EAX
		RET

_io_store_eflags:				; void io_store_eflags(int eflags); 写入eflags
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
		
_loadCr0:						; int loadCr0; 读取cr0寄存器
		MOV		EAX,CR0
		RET
		
_storeCr0:						; void storeCr0(int cr0) 写入cr0寄存器
		MOV		EAX,[ESP+4]
		MOV		CR0,EAX
		RET

_loadTr:						; void loadTr(int tr) 为TR寄存器赋值
		LTR		[ESP+4]
		RET
		
_asm_interruptHandler20:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_interruptHandler20
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD
		
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
		
_asm_consolePutchar:
		STI								; INT式调用本函数时回视作中断进行处理 CPU会自动关中断 在这里开中断
		PUSH	1
		AND		EAX,0xff				; 将AH 和EAX高位置0，将EAX置为已存入字符编码状态
		PUSH	EAX
		PUSH	DWORD [0x0fec]			; 读取内存0xfec并push该值（控制台进程保存的控制台信息）
		CALL	_consolePutchar			; 调用consolePutchar
		ADD		ESP,12					; 将栈中的数据丢弃
		IRETD							; INT中断式调用需要用IRETD返回

_memtest_sub:							; unsigned int memtest(unsigned int start, unsigned int end)
		PUSH	EDI
		PUSH	ESI
		PUSH	EBX						; 由于要使用这些寄存器，保存以上三个寄存器的值
		MOV		ESI,0xaa55aa55			; pat0 = 0xaa55aa55;
		MOV		EDI,0x55aa55aa			; pat1 = 0x55aa55aa;
		MOV		EAX,[ESP+12+4]			; i = start; 由于EAX是通用寄存器，不需要保存（EBX是32位基址寄存器，需要保存）当然保存也没问题
		
mts_loop:
		MOV		EBX,EAX
		ADD		EBX,0xffc				; p = i + 0xffc;
		MOV		EDX,[EBX]				; old = *p;
		MOV		[EBX],ESI				; *p = pat0;
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		EDI,[EBX]				; if(pat1 != *p) 
		JNE		mts_fin					; goto mts_fin;
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		ESI,[EBX]				; if(pat0 != *p)
		JNE		mts_fin					; goto mts_fin;
		MOV		[EBX],EDX				; *p = old;
		ADD 	EAX,0x1000				; i += 0x1000;
		CMP		EAX,[ESP+12+8]			; if(i <= end)
		JBE		mts_loop				; goto mts_loop;
		POP		EBX
		POP		ESI
		POP		EDI
		RET
		
mts_fin:
		MOV		[EBX],EDX				; *p = old;
		POP		EBX
		POP		ESI
		POP		EDI
		RET

_farJmp:								; void farJmp(int eip, int cs);
		JMP		FAR [ESP+4]
		RET

_farCall:								; void farCall(int eip, int cs);
		CALL	FAR [ESP+4]
		RET
