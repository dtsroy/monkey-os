[FORMAT "WCOFF"]				; 制作目标文件的模式	
[INSTRSET "i486p"]				; 使用到486为止的指令
[BITS 32]						; 3制作32位模式用的机器语言
[FILE "func.nas"]			; 文件名

		GLOBAL	_io_hlt

[SECTION .text] ;正式函数

_io_hlt: ; void io_hlt(void);
	HLT
	RET

_io_cli: ; void io_cli(void)
	CLI
	RET
