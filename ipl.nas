;启动区代码,FAT12

CYLS EQU 55 ;55柱面

ORG 0x7c00 ;装载规定

;FAT32专用

JMP entry
DB 0x90
DB "MONKEYOS" ;启动区名称,8字节
DW 512;
DB 1;
DW 1;;
DB 2;
DW 224;;
DW 2880;
DB 0xf0;
DW 9;
DW 18;
DW 2;
DD 0;
DD 2880;
DB 0,0,0x29;
DD 0xffffffff;
DB "MONKEYOS   " ;11字节
DB "FAT12   " ;8_
RESB 18 ;空18字节

;主体
entry:
	MOV AX,0
	MOV SS,AX
	MOV SP,0x7c00
	MOV DS,AX

	;读盘
	MOV AX,0x0820
	MOV ES,AX
	MOV CH,0 ;0柱面
	MOV DH,0 ;0磁头
	MOV CL,2 ;2扇区前一[1]个扇区装了启动区512字节

readloop:
	MOV SI,0 ;失败

retry: ;CHONGSHI
	MOV AH,0x02
	MOV AL,1 ;YIGE SHANQU
	MOV BX,0 ;HUANCHONG
	MOV DL,0x00 ;A驱动器
	INT 0x13 ;BIOS调用
	JNC next 
	ADD SI,1 ;失败+1
	CMP SI,5 ;失败和5比较
	JAE error ;SI>=5
	MOV AH,0x00
	MOV DL,0x00
	INT 0x13 ;重置
	JMP retry

next:
	MOV AX,ES
	ADD AX,0x0020
	MOV ES,AX
	ADD CL,1
	CMP CL,18
	JBE readloop
	MOV CL,1 ;重置到1扇区
	ADD DH,1
	CMP DH,2 ;磁头0,1
	JB readloop
	MOV DH,0 ;重置磁头到0(正面)
	ADD CH,1
	CMP CH,CYLS
	JB readloop
	MOV [0x0ff0],CH
	JMP 0xc200 ;转跳.sys

error:
	MOV SI,msg

putloop: ;打印文字
	MOV AL,[SI]
	ADD SI,1
	CMP AL,0
	JE exit ;退出
	MOV AH,0x0e
	MOV BX,15
	INT 0x10
	JMP putloop

exit:
	HLT
	JMP exit

msg:
	DB 0x0a, 0x0a, 0x0a
	DB "Monkey OS can't be loaded after trying 5 times.Try it again."
	DB 0x0a
	DB 0

	RESB 0x7dfe-$ ;填写0x00至0x001fe
	DB 0x55,0xaa ;标准结束,会检查最后两字节(512)
