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
		GLOBAL	_asm_interruptHandler0d
		GLOBAL	_memtest_sub
		GLOBAL	_farJmp, _farCall
		GLOBAL	_asm_dickApi, _startApp
		EXTERN	_interruptHandler20, _interruptHandler21, _interruptHandler27, _interruptHandler2c
		EXTERN	_interruptHandler0d
		EXTERN	_dickApi
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

_asm_interruptHandler0d:
		STI								; 开中断
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_interruptHandler0d
		CMP		EAX,0
		JNE		endApp
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		ADD		ESP,4
		IRETD

; 在寄存器EDX中存入功能号，可以通过INT调用不同函数
; 功能号1	显示单个字符(AL = 字符ascii码)
; 功能号2	显示字符串到0截止(EBX = 字符串地址)
; 功能号3	显示指定长度字符串(EBX = 字符串地址)
; 功能号4	结束应用程序
; dickApi返回0时继续运行应用程序，返回非0时当做TSS.esp0的地址处理，强制结束应用程序

_asm_dickApi:
		STI
		PUSH	DS
		PUSH	ES
		PUSHAD							; 保存寄存器的值
		PUSHAD							; 向dickApi传值
		MOV		AX,SS
		MOV		DS,AX					; 将操作系统用段地址存入DS和ES
		MOV		ES,AX
		CALL	_dickApi
		CMP		EAX,0					; EAX不为0时程序结束
		JNE		endApp
		ADD		ESP,32
		POPAD
		POP		ES
		POP		DS
		IRETD							; 自动STI
endApp:
; EAX为TSS.esp0的地址
		MOV		ESP,[EAX]
		POPAD
		RET								; 返回cmdApp

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
		
_startApp:								; void startApp(int eip, int cs, int esp, int ds, int *tssEsp0);
		PUSHAD							; 保存当前32位寄存器的值
		MOV		EAX,[ESP+36]			; EAX中存放eip
		MOV		ECX,[ESP+40]			; ECX中存放cs
		MOV		EDX,[ESP+44]			; EDX中存放esp
		MOV		EBX,[ESP+48]			; EBX中存放ds
		MOV		EBP,[ESP+52]			; TSS.esp0的地址
		MOV		[EBP],ESP				; 保存操作系统ESP
		MOV		[EBP+4],SS				; 保存操作系统SS
		MOV		ES,BX
		MOV		DS,BX
		MOV		FS,BX
		MOV		GS,BX
; 调整栈，以免RETF跳转至应用程序
		OR		ECX,3					; 将应用程序用的段号和3进行OR运算
		OR		EBX,3					; 将应用程序用的段号和3进行OR运算
		PUSH	EBX						; 应用程序ss
		PUSH	EDX						; 应用程序esp
		PUSH	ECX						; 应用程序cs
		PUSH	EAX						; 应用程序eip
		RETF
; 应用程序结束后不会回到这里
