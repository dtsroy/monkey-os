OBJS_KERNEL =	build/MonkeyOS.kernel.o	build/Functions.kernel.o	build/Graphic.kernel.o \
				build/font.kernel.o		build/Window.kernel.o		build/SystemStructure.kernel.o \
				build/Buffer.kernel.o	build/Sheet.kernel.o		build/Keyboard.kernel.o \
				build/Memory.kernel.o	build/Timer.kernel.o		build/Interruption.kernel.o \
				build/Task.kernel.o		build/Mouse.kernel.o		build/MonkeyStdlib.kernel.o \
				build/InitKernel.kernel.o

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
cc       = gcc -m32 -Os -I include/ -c -std=c99

VMX = "D:\\Program Files\\vms\\MonkeyOS\\MonkeyOS.vmx"

.PHONY:run
.PHONY:init
.PHONY:clean

run:
	$(make) $(TARGET_IMG)
	vmware -x $(VMX)

init:
	-mkdir build/
	-mkdir release/

clean:
	-rm -f build/*
	-rm -f release/*

$(TARGET_BOOT): startup/boot.asm Makefile
	@nasm -fbin $< -o $@
	@echo $@

build/asmhead.bin: startup/asmhead.asm Makefile
	@nasm -fbin $< -o $@
	@echo $@

$(TARGET_IMG): $(TARGET_BOOT) $(TARGET_SYS) Makefile
	@$(edimg)	imgin:$(fpdffile) \
				wbinimg src:$< len:512 from:0 to:0 \
				copy from:$(TARGET_SYS) to:@: \
				imgout:$@

build/kernel.bim: $(OBJS_KERNEL)
	@$(obj2bim) @$(rulefile) out:$@ stack:3136k map:build/kernel.map $^
	@echo $@

build/kernel.hrb: build/kernel.bim Makefile
	@$(bim2hrb) $< $@ 0
	@echo $@

$(TARGET_SYS): build/asmhead.bin build/kernel.hrb
	@cat $^ > $@
	@echo $@

build/font.kernel.o: $(fontfile) Makefile
	@$(mkf) $< build/font_.c
	@$(cc) build/font_.c -o $@
	@echo $@

build/%.kernel.o: kernel/%.c include/kernel/%.h Makefile
	@$(cc) $< -o $@
	@echo $@

build/MonkeyOS.kernel.o: kernel/MonkeyOS.c include/kernel/MonkeyOS.h Makefile # 注意:千万不要换成gcc -c,后果自负
	@cc1 $< -m32 -std=c99 -o build/MonkeyOS.kernel.s -I include/ -Os
	@echo 
	@as --32 build/MonkeyOS.kernel.s -o $@

build/Functions.kernel.o:kernel/Functions.asm Makefile
	@nasm -fwin32 $< -o $@
