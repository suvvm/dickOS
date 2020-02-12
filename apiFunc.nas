[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 使用486兼容指令集
[BITS 32]						; 生成32为模式机器语言
[FILE "helloCFunc.nas"]			; 源文件名信息

		GLOBAL	_apiPutchar, _apiPutstr0, _apiOpenWindow, _apiPutStrWin, _apiBoxFillWin, _apiInitMalloc, _apiMalloc, _apiFree
		GLOBAL	_apiAllocTimer, _apiInitTimer, _apiSetTimer, _apiFreeTimer, _apiBeep
		GLOBAL	_apiGetKey, _apiCloseWin, _apiLineWin, _apiPoint, _apiEnd, _apiRefreshWin
		
[SECTION .text]

_apiPutchar:					; void apiPutchar(int c)
		MOV		EDX,1			; 功能号1
		MOV		AL,[ESP+4]		; c
		INT		0x40
		RET
		
_apiPutstr0:					; void apiPutstr0(char *s)
		PUSH	EBX
		MOV		EDX,2
		MOV		EBX,[ESP+8]		; s
		INT		0x40
		POP		EBX
		RET
		
_apiOpenWindow:					; int apiOpenWindow(char *buf, int width, int height, int colInvNum, char *title);
		PUSH	EDI
		PUSH	ESI
		PUSH	EBX
		MOV		EDX,5			; 功能号5打印窗口
		MOV		EBX,[ESP+16]	; buf
		MOV		ESI,[ESP+20]	; width
		MOV		EDI,[ESP+24]	; height
		MOV		EAX,[ESP+28]	; colInvNum
		MOV		ECX,[ESP+32]	; title
		INT		0x40
		POP		EBX
		POP		ESI
		POP		EDI
		RET
		
_apiPutStrWin:					; void apiPutStrWin(int win, int x, int y, int col, int len, char *str);
		PUSH	EDI
		PUSH	ESI
		PUSH	EBP
		PUSH	EBX
		MOV		EDX,6			; 功能号6
		MOV		EBX,[ESP+20]	; win 图层句柄
		MOV		ESI,[ESP+24]	; x
		MOV		EDI,[ESP+28]	; y
		MOV		EAX,[ESP+32]	; col
		MOV		ECX,[ESP+36]	; len
		MOV		EBP,[ESP+40]	; str
		INT		0x40
		POP		EBX
		POP		EBP
		POP		ESI
		POP		EDI
		RET
		
_apiBoxFillWin:					; void apiBoxFillWin(int win, int startX, int startY, int endX, int endY, int col);
		PUSH	EDI
		PUSH	ESI
		PUSH	EBP
		PUSH	EBX
		MOV		EDX,7			; 功能号7
		MOV		EBX,[ESP+20]	; win 图层句柄
		MOV		EAX,[ESP+24]	; startX
		MOV		ECX,[ESP+28]	; startY
		MOV		ESI,[ESP+32]	; endX
		MOV		EDI,[ESP+36]	; endY
		MOV		EBP,[ESP+40]	; col
		INT		0x40
		POP		EBX
		POP		EBP
		POP		ESI
		POP		EDI
		RET
		
_apiInitMalloc:					; void apiInitMalloc();
		PUSH	EBX
		MOV		EDX,8			; 功能号8
		MOV		EBX,[CS:0x0020]	; 在应用程序文件中读取0x0020位置记录的当前malloc可分配内存空间起始地址（memsegtable管理空间起始地址）
		MOV		EAX,EBX
		ADD		EAX,32*1024		; 加上32KB
		MOV		ECX,[CS:0x0000]	; 在应用程序文件中读取0x0000位置读取数据段的大小（memsegtable管理的字节数）
		SUB		ECX,EAX
		INT		0x40
		POP		EBX
		RET

_apiMalloc:						; char *apiMalloc(int size);
		PUSH	EBX
		MOV		EDX,9			; 功能号9
		MOV		EBX,[CS:0x0020]
		MOV		ECX,[ESP+8]		; size
		INT		0x40
		POP		EBX
		RET
		
_apiFree:						; void apiFree(char *addr, int size);
		PUSH	EBX
		MOV		EDX,10			; 功能号10
		MOV		EBX,[CS:0x0020]
		MOV		EAX,[ESP+8]		; addr
		MOV		ECX,[ESP+12]	; size
		INT		0x40
		POP		EBX
		RET
		
_apiPoint:						; void apiPoint(int sheet, int x, int y, int col);
		PUSH	EDI
		PUSH	ESI
		PUSH	EBX
		MOV		EDX,11			; 功能号11
		MOV		EBX,[ESP+16]	; sheet图层句柄
		MOV		ESI,[ESP+20]	; x
		MOV		EDI,[ESP+24]	; y
		MOV		EAX,[ESP+28]	; col
		INT		0x40
		POP		EBX
		POP		ESI
		POP		EDI
		RET

_apiRefreshWin:					; void apiRefreshWin(int sheet, int startX, int startY, int endX, int endY);
		PUSH	EDI
		PUSH	ESI
		PUSH	EBX
		MOV		EDX,12			; 功能号12
		MOV		EBX,[ESP+16]	; sheet 图层句柄
		MOV		EAX,[ESP+20]	; startX
		MOV		ECX,[ESP+24]	; startY
		MOV		ESI,[ESP+28]	; endX
		MOV		EDI,[ESP+32]	; endY
		INT		0x40
		POP		EBX
		POP		ESI
		POP		EDI
		RET
		
_apiLineWin:					; void apiLineWin(int sheet, int startX, int startY, int endX, int endY, int col);
		PUSH	EDI
		PUSH	ESI
		PUSH	EBP
		PUSH	EBX
		MOV		EDX,13			; 功能号13
		MOV		EBX,[ESP+20]	; sheet
		MOV		EAX,[ESP+24]	; startX
		MOV		ECX,[ESP+28]	; startY
		MOV		ESI,[ESP+32]	; endX
		MOV		EDI,[ESP+36]	; endY
		MOV		EBP,[ESP+40]	; col
		INT		0x40
		POP		EBX
		POP		EBP
		POP		ESI
		POP		EDI
		RET
		
_apiCloseWin:					; void apiCloseWin(int sheet);
		PUSH	EBX
		MOV		EDX,14			; 功能号14
		MOV		EBX,[ESP+8]		; sheet
		INT		0x40
		POP		EBX
		RET
		
_apiGetKey:						; int apiGetKey(int mode);
		MOV		EDX,15
		MOV		EAX,[ESP+4]		; mode
		INT		0x40
		RET

_apiAllocTimer:					; int apiAllocTimer();
		MOV		EDX,16			; 功能号16
		INT		0x40
		RET
		
_apiInitTimer:					; void apiInitTimer(int timer, int data);
		PUSH	EBX
		MOV		EDX,17			; 功能号17
		MOV		EBX,[ESP+8]		; timer
		MOV		EAX,[ESP+12]	; data
		INT		0x40
		POP		EBX
		RET
		
_apiSetTimer:					; void apiSetTimer(int timer, int time);
		PUSH	EBX
		MOV		EDX,18			; 功能号18
		MOV		EBX,[ESP+8]		; timer
		MOV		EAX,[ESP+12]	; time
		INT		0x40
		POP		EBX
		RET

_apiFreeTimer:					; void apiFreeTimer(int timer);
		PUSH	EBX
		MOV		EDX,19			; 功能号19
		MOV		EBX,[ESP+8]		; timer
		INT		0x40
		POP		EBX
		RET
		
_apiBeep:						; void apiBeep(int tone);
		MOV		EDX,20			; 功能号20
		MOV		EAX,[ESP+4]		; tone
		INT		0x40
		RET

_apiEnd:						; void apiEnd()
		MOV		EDX,4
		INT		0x40
