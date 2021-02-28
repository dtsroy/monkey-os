[FORMAT "WCOFF"]				; 制作目标文件的模式	
[INSTRSET "i486p"]				; 使用到486为止的指令
[BITS 32]						; 3制作32位模式用的机器语言
[FILE "func.nas"]			; 文件名

		GLOBAL	_io_hlt, _io_cli, _io_outp8
		GLOBAL _io_load_eflags, _io_save_eflags
		GLOBAL _load_gdtr, _load_idtr
		GLOBAL _ihr21x, _ihr27x

		EXTERN _ihr21, _ihr27

[SECTION .text] ;正式函数

_io_hlt: ; void io_hlt(void);
	HLT
	RET

_io_cli: ; void io_cli(void)
	CLI
	RET

_io_outp8: ; void io_outp8(int port, int data);
	MOV EDX,[ESP+4]
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
		CALL	_inthandler27
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

