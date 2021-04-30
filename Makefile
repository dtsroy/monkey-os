TOOLPATH = ..\\z_tools\\
INCPATH  = ../z_tools/haribote/

OBJS_MP = mkpack.o func.o graphic.o \
			tools.o font.o init_dt.o int.o \
			fifo.o keyboard_and_mouse.o memory.o \
			sheet.o window.o timer.o mktask.o

DEL = python bash.py del
COPY = python bash.py copy

make     = $(TOOLPATH)make.exe -r
nask     = $(TOOLPATH)nask.exe
cc1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -w -quiet -std=c99
gas2nask = $(TOOLPATH)gas2nask.exe -a
obj2bim  = $(TOOLPATH)obj2bim.exe
bim2hrb  = $(TOOLPATH)bim2hrb.exe
rulefile = rulefile.txt
edimg    = $(TOOLPATH)edimg.exe

default :
	$(make) img

# 镜像文件生成

ipl.bin : ipl.nas Makefile
	$(nask) ipl.nas ipl.bin ipl.lst

asmhead.bin : asmhead.nas Makefile
	$(nask) asmhead.nas asmhead.bin

MonkeyOS.img : ipl.bin MonkeyOS.sys Makefile
	$(edimg)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:ipl.bin len:512 from:0 to:0 \
		copy from:MonkeyOS.sys to:@: \
		imgout:MonkeyOS.img

mkpack.bim : $(OBJS_MP) Makefile
	$(obj2bim) @$(rulefile) out:mkpack.bim stack:3136k map:mkpack.map \
		$(OBJS_MP)
# 3MB+64KB=3136KB

mkpack.hrb : mkpack.bim Makefile
	$(bim2hrb) mkpack.bim mkpack.hrb 0

MonkeyOS.sys : asmhead.bin mkpack.hrb Makefile
	$(COPY) /B asmhead.bin+mkpack.hrb MonkeyOS.sys

font.bin : font.txt Makefile
	makefont font.txt font.bin

font.o : font.bin Makefile
	bin2obj font.bin font.o _xfont

%.gas : %.c mkpack.h Makefile
	$(cc1) -o $*.gas $*.c

%.nas : %.gas Makefile
	$(gas2nask) $*.gas $*.nas

%.o : %.nas Makefile
	$(nask) $*.nas $*.o $*.lst

asm :
	$(make) ipl.bin

img :
	$(make) MonkeyOS.img

run :
	$(make) img
# 	$(COPY) MonkeyOS.img ..\z_tools\qemu\fdimage0.bin
# 	$(make) -C ../z_tools/qemu

install :
	$(make) img
	../z_tools/imgtol.com w a: MonkeyOS.img

clean :
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.gas
	-$(DEL) *.o
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.gas
	-$(DEL) *.o
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.gas
	-$(DEL) *.o
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.gas
	-$(DEL) *.o
	-$(DEL) mkpack.nas
	-$(DEL) mkpack.map
	-$(DEL) mkpack.bim
	-$(DEL) mkpack.hrb
	-$(DEL) MonkeyOS.sys

src_only :
	$(make) clean
	-del MonkeyOS.img
