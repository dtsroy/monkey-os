[BITS 32]						; 3制作32位模式用的机器语言

		GLOBAL	_io_hlt,	_io_cli,	_io_sti,	_io_outp8
		GLOBAL _io_load_eflags,			_io_save_eflags
		GLOBAL _load_gdtr,	_load_idtr
		GLOBAL _ihr21x,		_ihr27x,	_ihr2cx,	_ihr20x
		GLOBAL _io_inp8,	_io_shlt,	_load_cr0,	_save_cr0
		GLOBAL _getmemx,	_load_tr,	_jmpfar,	_fin

		EXTERN _ihr21, _ihr27, _ihr2c, _ihr20

[SECTION .text] ;正式函数

_io_hlt: ; void io_hlt(void);
	HLT
	RET

_io_cli: ; void io_cli(void)
	CLI
	RET

_io_sti:
	STI
	RET

_io_outp8: ; void io_outp8(int port, int data);
	MOV DX,[ESP+4]
	MOV AL,[ESP+8]
	OUT DX,AL
	RET

_io_load_eflags:
	;载入eflags
	PUSHFD
	POP EAX
	RET

_io_save_eflags:
	MOV EAX,[ESP+4]
	PUSH EAX
	POPFD
	RET

_load_gdtr:		; void load_gdtr(int limit, int addr);
	MOV		AX,[ESP+4]		; limit
	MOV		[ESP+6],AX
	LGDT	[ESP+6]
	RET

_load_idtr:		; void load_idtr(int limit, int addr);
	MOV		AX,[ESP+4]		; limit
	MOV		[ESP+6],AX
	LIDT	[ESP+6]
	RET

_ihr21x:
	PUSH	ES
	PUSH	DS
	PUSHAD
	MOV		EAX,ESP
	PUSH	EAX
	MOV		AX,SS
	MOV		DS,AX
	MOV		ES,AX
	CALL	_ihr21
	POP		EAX
	POPAD
	POP		DS
	POP		ES
	IRETD

_ihr27x:
	PUSH	ES
	PUSH	DS
	PUSHAD
	MOV		EAX,ESP
	PUSH	EAX
	MOV		AX,SS
	MOV		DS,AX
	MOV		ES,AX
	CALL	_ihr27
	POP		EAX
	POPAD
	POP		DS
	POP		ES
	IRETD

_ihr2cx:
	PUSH	ES
	PUSH	DS
	PUSHAD
	MOV		EAX,ESP
	PUSH	EAX
	MOV		AX,SS
	MOV		DS,AX
	MOV		ES,AX
	CALL	_ihr2c
	POP		EAX
	POPAD
	POP		DS
	POP		ES
	IRETD

_ihr20x:
	PUSH	ES
	PUSH	DS
	PUSHAD
	MOV		EAX,ESP
	PUSH	EAX
	MOV		AX,SS
	MOV		DS,AX
	MOV		ES,AX
	CALL	_ihr20
	POP		EAX
	POPAD
	POP		DS
	POP		ES
	IRETD

_io_inp8:	; int io_inp8(int port);
	MOV		EDX,[ESP+4]		; port
	MOV		EAX,0
	IN		AL,DX
	RET

_io_shlt:
	STI
	HLT
	RET

_load_cr0:
	MOV EAX,CR0
	RET

_save_cr0:
	MOV EAX,[ESP+4]
	MOV CR0,EAX
	RET

_getmemx:
	PUSH EDI
	PUSH ESI
	PUSH EBX ;压栈
	MOV ESI,0x11221122 
	MOV EDI,0xeeddeedd ;0x11221122 XOR 0xffffffff
	MOV EAX,[ESP+16]
gmlp:
	MOV EBX,EAX
	ADD EBX,0x7ffc
	MOV EDX,[EBX]
	MOV [EBX],ESI
	XOR DWORD [EBX],0xffffffff
	CMP EDI,[EBX]
	JNE gmfin
	XOR DWORD [EBX],0xffffffff
	CMP ESI,[EBX]
	JNE gmfin
	MOV [EBX],EDX
	ADD EAX,0x8000
	CMP EAX,[ESP+20]
	JBE gmlp
	POP EBX
	POP ESI
	POP EDI
	RET
gmfin:
	MOV [EBX],EDX
	POP EBX
	POP ESI
	POP EDI
	RET

_load_tr:
	LTR [ESP+4]
	RET

_jmpfar:
	JMP FAR [ESP+4]
	RET

_fin:
	HLT
	JMP _fin
	RET
