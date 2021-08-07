OBJS_KERNEL =	build/MonkeyOS.kernel.o	build/Functions.kernel.o	build/Graphic.kernel.o \
				build/font.kernel.o		build/Window.kernel.o		build/SystemStructure.kernel.o \
				build/Buffer.kernel.o	build/Sheet.kernel.o		build/keyboard_and_mouse.kernel.o \
				build/Memory.kernel.o	build/Timer.kernel.o		build/Interruption.kernel.o \
				build/Task.kernel.o

rulefile = res/rulefile.txt
fontfile = res/font.txt
fpdffile = res/default.fp

TARGET_SYS  = release/MonkeyOS.sys
TARGET_IMG  = release/MonkeyOS.img
TARGET_BOOT = release/boot.bsf

del      = python tools/bash.py del
copy     = python tools/bash.py copy
mkf      = python tools/makefont.py
make     = ../z_tools/make.exe # 注意:这个千万不要改
obj2bim  = ../z_tools/obj2bim.exe
bim2hrb  = ../z_tools/bim2hrb.exe
edimg    = ../z_tools/edimg.exe

VMX = "D:\\Program Files\\vms\\test_mbk\\BackupTest.vmx"

run:
	$(make) $(TARGET_IMG)
	vmware -x $(VMX)

init:
	-mkdir build/
	-mkdir release/

$(TARGET_BOOT): startup/boot.asm Makefile
	nasm -fbin $< -o $@

build/asmhead.bin: startup/asmhead.asm Makefile
	nasm -fbin $< -o $@

$(TARGET_IMG): $(TARGET_BOOT) $(TARGET_SYS) Makefile
	$(edimg)	imgin:$(fpdffile) \
				wbinimg src:$< len:512 from:0 to:0 \
				copy from:$(TARGET_SYS) to:@: \
				imgout:$@

build/kernel.bim: $(OBJS_KERNEL)
	$(obj2bim) @$(rulefile) out:$@ stack:3136k map:build/kernel.map $^

build/kernel.hrb: build/kernel.bim Makefile
	$(bim2hrb) $< $@ 0

$(TARGET_SYS): build/asmhead.bin build/kernel.hrb
	cat $^ > $@

build/font.kernel.o: $(fontfile) Makefile
	$(mkf) $< build/font_.c
	gcc -c build/font_.c -o $@ -m32

build/%.kernel.o: kernel/%.c Makefile
	# 注意:千万不要换成gcc -c,后果自负
	cc1 $< -m32 -std=c99 -o build/$*.kernel.s -I include/
	as --32 build/$*.kernel.s -o $@

build/Functions.kernel.o:kernel/Functions.asm Makefile
	nasm -fwin32 $< -o $@
